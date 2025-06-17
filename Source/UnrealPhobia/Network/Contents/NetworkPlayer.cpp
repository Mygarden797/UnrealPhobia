#include "NetworkPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "NetworkBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "StaminaBar.h"
#include "MentalBar.h"

ANetworkPlayer::ANetworkPlayer()
{
    // Set collision capsule size
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    // Character movement configuration
    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
    GetCharacterMovement()->bUseControllerDesiredRotation = true;
    GetCharacterMovement()->MaxWalkSpeed = 400.f;
    GetCharacterMovement()->MaxWalkSpeedCrouched = 200.f;
    GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

    GetCharacterMovement()->JumpZVelocity = 700.f;
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
    GetCharacterMovement()->bRunPhysicsWithNoController = true;

    // Create camera boom (pulls in towards the player if there is a collision)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 200.0f;
    CameraBoom->SocketOffset = FVector(0.f, 45.f, 30.f);
    CameraBoom->ProbeSize = 12.f;
    CameraBoom->bUsePawnControlRotation = true;

    // Create a follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;
}

void ANetworkPlayer::BeginPlay()
{
    Super::BeginPlay();

    // Initialize stats
    CurrentStamina = MaxStamina;
    CurrentMental = MaxMental;

    // Setup input mapping context
    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
              Subsystem->AddMappingContext(SurvivorMovingContext, 0);
        }
    }

    // Only setup UI for locally controlled players
    if (IsLocallyControlled())
    {
        // Create crosshair widget
        if (CrosshairWidgetClass)
        {
            CrosshairWidget = CreateWidget<UUserWidget>(GetWorld(), CrosshairWidgetClass);
            if (CrosshairWidget)
            {
                CrosshairWidget->AddToViewport();
            }
        }

        // Create stamina bar widget
        if (StaminaBarClass)
        {
            StaminaBar = CreateWidget<UStaminaBar>(GetWorld(), StaminaBarClass);
            if (StaminaBar)
            {
                StaminaBar->AddToViewport();
            }
        }

        // Create mental bar widget
        if (MentalBarClass)
        {
            MentalBar = CreateWidget<UMentalBar>(GetWorld(), MentalBarClass);
            if (MentalBar)
            {
                MentalBar->AddToViewport();
            }
        }

        // Create inventory widget
        if (InventoryWidgetClass)
        {
            InventoryWidget = CreateWidget<UUserWidget>(GetWorld(), InventoryWidgetClass);
            if (InventoryWidget)
            {
                InventoryWidget->AddToViewport();
            }
        }
    }

    // Mental decay timer
    GetWorldTimerManager().SetTimer(
        MentalDecayTimerHandle,
        this,
        &ANetworkPlayer::DecreaseMental,
        1.0f,
        true);

    // Deactivate all CandleRooms
    TArray<AActor*> FoundTriggers;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("CandleRoom"), FoundTriggers);

    for (AActor* Trigger : FoundTriggers)
    {
        Trigger->Tags.Remove(FName("Active"));
    }

    ActivateRandomMentalTrigger();
}

void ANetworkPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComp = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // Network movement bindings
        EnhancedInputComp->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
        EnhancedInputComp->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
        EnhancedInputComp->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ANetworkPlayer::Move);
        EnhancedInputComp->BindAction(MoveAction, ETriggerEvent::Completed, this, &ANetworkPlayer::Move);
        EnhancedInputComp->BindAction(LookAction, ETriggerEvent::Triggered, this, &ANetworkPlayer::Look);

        // Survivor-specific bindings
        //if (MoveForwardAction)
        //{
        //    EnhancedInputComp->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &ANetworkPlayer::MoveForward);
        //}
        //if (MoveRightAction)
        //{
        //    EnhancedInputComp->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &ANetworkPlayer::MoveRight);
        //}
        EnhancedInputComp->BindAction(SprintAction, ETriggerEvent::Started, this, &ANetworkPlayer::Sprint);
        EnhancedInputComp->BindAction(SprintAction, ETriggerEvent::Completed, this, &ANetworkPlayer::Sprint);

        EnhancedInputComp->BindAction(CrouchAction, ETriggerEvent::Started, this, &ANetworkPlayer::SetCrouch);
        EnhancedInputComp->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ANetworkPlayer::SetCrouch);

    }
}

void ANetworkPlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Network packet sending logic (from MyProtoPlayer)
    if (IsMyPlayer())
    {
        bool ForceSendPacket = false;

        if (LastDesiredInput != DesiredInput)
        {
            ForceSendPacket = true;
            LastDesiredInput = DesiredInput;
        }

        // State information
        if (DesiredInput == FVector2D::Zero())
            SetMoveState(Protocol::MOVE_STATE_IDLE);
        else
            SetMoveState(Protocol::MOVE_STATE_RUN);

        MovePacketSendTimer -= DeltaTime;

        if (MovePacketSendTimer <= 0 || ForceSendPacket)
        {
            MovePacketSendTimer = MOVE_PACKET_SEND_DELAY;

            Protocol::C_MOVE MovePkt;

            // Current position information
            {
                Protocol::PosInfo* Info = MovePkt.mutable_info();
                Info->CopyFrom(*PlayerInfo);
                Info->set_yaw(DesiredYaw);
                Info->set_state(GetMoveState());
            }

            SEND_PACKET(MovePkt);
        }
    }
}

// Network Movement Functions (from MyProtoPlayer)
void ANetworkPlayer::Move(const FInputActionValue& Value)
{
    FVector2D MovementVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        AddMovementInput(ForwardDirection, MovementVector.Y);
        AddMovementInput(RightDirection, MovementVector.X);

        // Cache for network
        {
            DesiredInput = MovementVector;

            DesiredMoveDirection = FVector::ZeroVector;
            DesiredMoveDirection += ForwardDirection * MovementVector.Y;
            DesiredMoveDirection += RightDirection * MovementVector.X;
            DesiredMoveDirection.Normalize();

            const FVector Location = GetActorLocation();
            FRotator Rotator = UKismetMathLibrary::FindLookAtRotation(Location, Location + DesiredMoveDirection);
            DesiredYaw = Rotator.Yaw;
        }
    }
}

void ANetworkPlayer::Look(const FInputActionValue& Value)
{
    FVector2D LookAxisVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        AddControllerYawInput(LookAxisVector.X);
        AddControllerPitchInput(LookAxisVector.Y);
    }
}

// Survivor Movement Functions
//void ANetworkPlayer::MoveForward(const FInputActionValue& Value)
//{
//    const float AxisValue = Value.Get<float>();
//    if (Controller && AxisValue != 0.0f)
//    {
//        const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
//        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
//        AddMovementInput(Direction, AxisValue);
//    }
//}
//
//void ANetworkPlayer::MoveRight(const FInputActionValue& Value)
//{
//    const float AxisValue = Value.Get<float>();
//
//    if (Controller && AxisValue != 0.0f)
//    {
//        const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
//        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
//        AddMovementInput(Direction, AxisValue);
//    }
//}

// Sprint Functions
void ANetworkPlayer::Sprint(const FInputActionValue& Value)
{
    const bool DoSprint = Value.Get<bool>();

    if (DoSprint && CurrentStamina > 0.f)
    {
        StartSprint();
    }
    else
    {
        StopSprint();
    }
}

void ANetworkPlayer::StartSprint()
{
    if (bIsSprinting || CurrentStamina <= 0.f)
        return;

    bIsSprinting = true;
    GetCharacterMovement()->MaxWalkSpeed = 900.f;

    if (!GetWorldTimerManager().IsTimerActive(FStaminaLossHandle))
    {
        GetWorldTimerManager().ClearTimer(FStaminaLossHandle);
        GetWorldTimerManager().SetTimer(
            FStaminaLossHandle,
            this,
            &ANetworkPlayer::LossStamina,
            0.1f,
            true);
    }
}

void ANetworkPlayer::StopSprint()
{
    bIsSprinting = false;
    GetCharacterMovement()->MaxWalkSpeed = 400.f;

    GetWorldTimerManager().ClearTimer(FStaminaLossHandle);

    if (CurrentStamina < MaxStamina)
    {
        GetWorldTimerManager().SetTimer(
            FStaminaRegenHandle,
            this,
            &ANetworkPlayer::RegenStamina,
            0.1f,
            true);
    }
}

void ANetworkPlayer::LossStamina()
{
    if (!bIsSprinting || GetVelocity().SizeSquared() <= 0.0f)
    {
        return;
    }

    if (CurrentStamina <= StaminaLossRate * 0.1f)
    {
        CurrentStamina = 0.0f;
        StopSprint();
    }
    else
    {
        CurrentStamina -= StaminaLossRate * 0.1f;
    }
    UpdateStaminaBar();
}

void ANetworkPlayer::RegenStamina()
{
    if (bIsSprinting || CurrentStamina >= MaxStamina)
    {
        GetWorldTimerManager().ClearTimer(FStaminaRegenHandle);
        return;
    }
    else
    {
        CurrentStamina += StaminaRegenRate * 0.1f;
        CurrentStamina = FMath::Min(CurrentStamina, MaxStamina);
    }

    UpdateStaminaBar();
}

// Crouch Function
void ANetworkPlayer::SetCrouch(const FInputActionValue& value)
{
    const bool bPressed = value.Get<bool>();
    if (bPressed)
    {
        bIsCrouch = true;
        Crouch();
    }
    else
    {
        bIsCrouch = false;
        UnCrouch();
    }
}

// Mental Functions
void ANetworkPlayer::IncreaseMental(float Amount)
{
    CurrentMental = FMath::Clamp(CurrentMental + Amount, 0.f, MaxMental);
    UpdateMentalBar();
}

void ANetworkPlayer::DecreaseMental(float Amount)
{
    CurrentMental = FMath::Clamp(CurrentMental - Amount, 0.f, MaxMental);
    UpdateMentalBar();
}

void ANetworkPlayer::DecreaseMental()
{
    if (CurrentMental > 0.f)
    {
        CurrentMental -= MentalDecayRate;
        CurrentMental = FMath::Clamp(CurrentMental, 0.f, MaxMental);

        if (CurrentMental <= 0.f)
        {
            GameOver();
        }
    }
    UpdateMentalBar();
}

void ANetworkPlayer::GameOver()
{
    UE_LOG(LogTemp, Warning, TEXT("Game Over! Mental is Zero."));
}

//공포상태 진입이 여기밖에 없음. 조정할 필요 있음. 데미지 입는 것도 DecreaseMental 함수 이용하도록 고치기
float ANetworkPlayer::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    if (bIsDead)
        return 0.0f;

    if (bIsFear && CurrentMental <= 0.0f)
    {
        GameOver();
        return 0.0f;
    }

    // 데미지를 적용
    float AppliedDamage = FMath::Clamp(DamageAmount, 0.0f, CurrentMental);
    CurrentMental -= AppliedDamage;

    // 정신력이 0 이하로 떨어졌을 경우 공포 상태 진입
    if (CurrentMental <= 0.0f)
    {
        CurrentMental = 0.0f;  // 음수 방지 (불필요하면 생략 가능)
        bIsFear = true;
    }
    UpdateMentalBar();
    return AppliedDamage;
}



void ANetworkPlayer::StartMentalRegen(float Duration)
{
    bIsInMentalRegenZone = true;

    GetWorldTimerManager().SetTimer(MentalRegenTimerHandle, this, &ANetworkPlayer::RegenMental, MentalRegenTickTime, true);
    GetWorldTimerManager().SetTimer(MentalRegenDurationHandle, this, &ANetworkPlayer::StopMentalRegen, Duration, false);
}

void ANetworkPlayer::StopMentalRegen()
{
    bIsInMentalRegenZone = false;
    GetWorldTimerManager().ClearTimer(MentalRegenTimerHandle);
    GetWorldTimerManager().ClearTimer(MentalRegenDurationHandle);

    if (CurrentTrigger)
    {
        CurrentTrigger->Tags.Remove(FName("Active"));
    }

    ActivateRandomMentalTrigger();
}

void ANetworkPlayer::RegenMental()
{
    if (CurrentMental >= MaxMental)
    {
        StopMentalRegen();
        return;
    }

    CurrentMental = FMath::Clamp(CurrentMental + MentalRegenPerTick, 0.f, MaxMental);
    UpdateMentalBar();
}

void ANetworkPlayer::ActivateRandomMentalTrigger()
{
    TArray<AActor*> FoundTriggers;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("CandleRoom"), FoundTriggers);

    TArray<AActor*> AvailableTriggers;
    for (AActor* Trigger : FoundTriggers)
    {
        if (Trigger != CurrentTrigger)
        {
            AvailableTriggers.Add(Trigger);
        }
    }

    if (AvailableTriggers.Num() > 0)
    {
        int32 Index = FMath::RandRange(0, AvailableTriggers.Num() - 1);
        AActor* SelectedTrigger = AvailableTriggers[Index];
        CurrentTrigger = SelectedTrigger;
        SelectedTrigger->Tags.AddUnique(FName("Active"));

        UE_LOG(LogTemp, Log, TEXT("Activated Mental Trigger: %s"), *SelectedTrigger->GetName());
    }
}

// UI Update Functions
void ANetworkPlayer::UpdateStaminaBar()
{
    if (StaminaBar)
    {
        float Percent = CurrentStamina / MaxStamina;
        StaminaBar->SetStaminaPercent(Percent);
    }
}

void ANetworkPlayer::UpdateMentalBar()
{
    if (MentalBar)
    {
        float Percent = CurrentMental / MaxMental;
        MentalBar->SetMentalPercent(Percent);
    }
}