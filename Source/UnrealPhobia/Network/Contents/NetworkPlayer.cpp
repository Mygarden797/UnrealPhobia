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
#include "InventoryWidget.h"
#include "NetworkManager.h"
#include "StaminaBar.h"
#include "MentalBar.h"
#include "Mental/CandleRoom.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Creature/CreatureBase.h"
#include "TimerManager.h"
#include "EngineUtils.h"

ANetworkPlayer::ANetworkPlayer(const FObjectInitializer &ObjectInitializer)
{
    // Set collision capsule size
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    // Character movement configuration
    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
    GetCharacterMovement()->bUseControllerDesiredRotation = true;
    GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
    GetCharacterMovement()->MaxWalkSpeedCrouched = GetCharacterMovement()->MaxWalkSpeed * 0.5f;
    GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

    GetCharacterMovement()->JumpZVelocity = 700.f;
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
    GetCharacterMovement()->bRunPhysicsWithNoController = true;

    // Create camera boom (pulls in towards the player if there is a collision)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 90.0f;
    CameraBoom->SocketOffset = FVector(10.f, 45.f, 0.f);
    CameraBoom->ProbeSize = 12.f;
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->bEnableCameraLag = true;
    CameraBoom->bUseCameraLagSubstepping = true;

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

        if (InventoryWidgetClass)
        {
            InventoryWidget = CreateWidget<UInventoryWidget>(GetWorld(), InventoryWidgetClass);
            if (InventoryWidget)
            {
                InventoryWidget->AddToViewport();
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to load Inventory!!"));
        }
    }

    // Mental decay timer
    GetWorldTimerManager().SetTimer(
        MentalDecayTimerHandle,
        this,
        &ANetworkPlayer::DecreaseMental,
        1.0f,
        true);

    TArray<AActor *> FoundTriggers;
    TArray<AActor *> FoundTriggers2;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("CandleRoom"), FoundTriggers);
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("CandleRoom2"), FoundTriggers2);

    TArray<AActor *> AvailableTriggers;
    for (AActor *Trigger : FoundTriggers)
    {
        if (Trigger != CurrentCandleRoom)
        {
            AvailableTriggers.Add(Trigger);
        }
    }

    TArray<AActor *> AvailableTriggers2;
    for (AActor *Trigger : FoundTriggers2)
    {
        if (Trigger != CurrentCandleRoom)
        {
            AvailableTriggers2.Add(Trigger);
        }
    }

    // CandleRoom Ʈ ̸
    UE_LOG(LogTemp, Log, TEXT("=== FoundTriggers (CandleRoom) ==="));
    for (AActor *Trigger : FoundTriggers)
    {
        if (Trigger)
        {
            UE_LOG(LogTemp, Log, TEXT("Trigger Name: %s"), *Trigger->GetName());
        }
    }

    // CandleRoom2 Ʈ ̸
    UE_LOG(LogTemp, Log, TEXT("=== FoundTriggers2 (CandleRoom2) ==="));
    for (AActor *Trigger : FoundTriggers2)
    {
        if (Trigger)
        {
            UE_LOG(LogTemp, Log, TEXT("Trigger Name: %s"), *Trigger->GetName());
        }
    }

    if (AvailableTriggers.Num() > 0)
    {
        int32 Index = FMath::RandRange(0, AvailableTriggers.Num() - 1);
        int32 Index2 = FMath::RandRange(0, AvailableTriggers2.Num() - 1);
        AActor *SelectedTrigger = AvailableTriggers[Index];
        AActor *SelectedTrigger2 = AvailableTriggers2[Index2];
        SelectedTrigger->Tags.AddUnique(FName("Active")); // Activate with Active Tag
        SelectedTrigger2->Tags.AddUnique(FName("Active"));

        UE_LOG(LogTemp, Log, TEXT("Activated Mental Trigger: %s"), *SelectedTrigger->GetName());
        UE_LOG(LogTemp, Log, TEXT("Activated Mental Trigger: %s"), *SelectedTrigger2->GetName());

        if (ACandleRoom *CandleRoom = Cast<ACandleRoom>(SelectedTrigger))
        {
            CandleRoom->TurnOnEffects();
            UE_LOG(LogTemp, Log, TEXT("Candle flames turned ON for: %s"), *SelectedTrigger->GetName());
        }

        if (ACandleRoom *CandleRoom = Cast<ACandleRoom>(SelectedTrigger2))
        {
            CandleRoom->TurnOnEffects();
            UE_LOG(LogTemp, Log, TEXT("Candle flames turned ON for: %s"), *SelectedTrigger2->GetName());
        }
    }

    // 공격 카메라 위젯 생성
    if (AttackCameraWidgetClass)
    {
        AttackCameraWidget = CreateWidget<UAttackCameraWidget>(GetWorld(), AttackCameraWidgetClass);
        if (AttackCameraWidget)
        {
            AttackCameraWidget->AddToViewport();
        }
    }
    // 크리쳐 공격 카메라 델리게이트 바인딩 추가
    if (!ACreatureBase::OnCreatureAttackCamera.IsBoundToObject(this))
    {
        ACreatureBase::OnCreatureAttackCamera.AddUObject(this, &ANetworkPlayer::OnCreatureAttackCamera);
    }
}

void ANetworkPlayer::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (APlayerController *PlayerController = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem *Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(SurvivorMovingContext, 0);
            UE_LOG(LogTemp, Display, TEXT("Moving Key is ready"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to Mapping Keys"));
    }

    if (UEnhancedInputComponent *EIC = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ANetworkPlayer::Move);
        EIC->BindAction(MoveAction, ETriggerEvent::Completed, this, &ANetworkPlayer::MoveReleased);
        EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &ANetworkPlayer::Look);
        EIC->BindAction(SwitchViewAction, ETriggerEvent::Started, this, &ANetworkPlayer::SwitchCameraView);

        EIC->BindAction(SprintAction, ETriggerEvent::Started, this, &ANetworkPlayer::Sprint);
        EIC->BindAction(SprintAction, ETriggerEvent::Completed, this, &ANetworkPlayer::Sprint);

        EIC->BindAction(CrouchAction, ETriggerEvent::Started, this, &ANetworkPlayer::SetCrouch);
        EIC->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ANetworkPlayer::SetCrouch);
        UE_LOG(LogTemp, Display, TEXT("Key Binding is done"));

		if (ForceActivateAction)
		{
			EIC->BindAction(ForceActivateAction, ETriggerEvent::Started, this, &ANetworkPlayer::OnForceActivatePressed);
			EIC->BindAction(ForceActivateAction, ETriggerEvent::Completed, this, &ANetworkPlayer::OnForceActivateReleased);
		}
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to Bind Input!!"));
        return;
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

        // 입력과 실제 속도 모두 고려
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
                Protocol::PosInfo *Info = MovePkt.mutable_info();
                Info->CopyFrom(*PlayerInfo);
                Info->set_yaw(DesiredYaw);
                Info->set_state(GetMoveState());
                Info->set_crouch(Protocol::CROUCH_STATE_CROUCH);
                //if (bIsCrouch)
                //
                //    Info->set_crouch(Protocol::CROUCH_STATE_CROUCH);
                //
            }

            SEND_PACKET(MovePkt);
        }
    }
}

// Network Movement Functions (from MyProtoPlayer)
void ANetworkPlayer::Move(const FInputActionValue &Value)
{
    FVector2D MovementVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        FVector MoveDir = (ForwardDirection * MovementVector.Y + RightDirection * MovementVector.X).GetSafeNormal();
        UpdateDirectionWeight(MoveDir);
        AddMovementInput(MoveDir);

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

void ANetworkPlayer::MoveReleased()
{
    // 키를 떼었을 때 DesiredInput을 0으로 설정
    DesiredInput = FVector2D::Zero();
    DesiredMoveDirection = FVector::ZeroVector;
}

void ANetworkPlayer::Look(const FInputActionValue &Value)
{
    FVector2D LookAxisVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        AddControllerYawInput(LookAxisVector.X);
        AddControllerPitchInput(LookAxisVector.Y * -1);
    }
}

// Survivor Movement Functions
// void ANetworkPlayer::MoveForward(const FInputActionValue& Value)
//
//    const float AxisValue = Value.Get<float>();
//    if (Controller && AxisValue != 0.0f)
//    
//        const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
//        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
//        AddMovementInput(Direction, AxisValue);
//    
//
//
// void ANetworkPlayer::MoveRight(const FInputActionValue& Value)
//
//    const float AxisValue = Value.Get<float>();
//
//    if (Controller && AxisValue != 0.0f)
//    
//        const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
//        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
//        AddMovementInput(Direction, AxisValue);
//    
//

// Sprint Functions
void ANetworkPlayer::Sprint(const FInputActionValue &Value)
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
    if (bIsCrouch)
        return;
    if (bIsSprinting || CurrentStamina <= 0.f)
        return;

    bIsSprinting = true;
    UpdateCameraLag();
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
    UpdateCameraLag();
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

void ANetworkPlayer::UpdateDirectionWeight(FVector MoveDir)
{
    float Dot = FVector::DotProduct(GetActorForwardVector(), MoveDir);

    float SpeedMultiplier;

    if (Dot >= 0.95f)
    {
        SpeedMultiplier = 1.2f;
    }
    else if (Dot < -0.2f)
    {
        SpeedMultiplier = 0.8f;
    }
    else
    {
        SpeedMultiplier = 1.0f;
    }

    if (bIsSprinting && CurrentStamina > 0.0f)
    {
        GetCharacterMovement()->MaxWalkSpeed = (BaseSpeed * SpeedMultiplier) * 1.5f;
        // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Run"));
    }
    else
    {
        GetCharacterMovement()->MaxWalkSpeed = (BaseSpeed * SpeedMultiplier);
    }
    GetCharacterMovement()->MaxWalkSpeedCrouched = GetCharacterMovement()->MaxWalkSpeed * 0.5f;

    // UE_LOG(LogTemp, Display, TEXT("Dot: %.3f | Speed: %f, %f "),
    //       Dot, GetCharacterMovement()->MaxWalkSpeed, GetCharacterMovement()->MaxWalkSpeedCrouched);
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
void ANetworkPlayer::SetCrouch(const FInputActionValue &value)
{
    const bool bPressed = value.Get<bool>();

    if (bIsSprinting)
    {
        bIsSprinting = false;
    }
    if (bPressed)
    {
        bIsCrouch = true;
        UpdateCameraLag();
        Crouch();
    }
    else
    {
        bIsCrouch = false;
        UpdateCameraLag();
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
        // UE_LOG(LogTemp, Log, TEXT("CurrentMental: %f"), CurrentMental);

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
    APlayerController *PlayerController = Cast<APlayerController>(GetController());
    GameOverWidget = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);
    if (GameOverWidgetClass)
    {
        GameOverWidget->AddToViewport();
        if (PlayerController)
        {
            DisableInput(PlayerController);
            PlayerController->bShowMouseCursor = true;
        }
    }

    Protocol::C_DEFEAT Pkt;

    SEND_PACKET(Pkt);

    UNetworkManager *GameInstance = Cast<UNetworkManager>(GetGameInstance());
    GameInstance->DisconnectFromGameServer();
}

void ANetworkPlayer::GameWin()
{
    UE_LOG(LogTemp, Warning, TEXT("Game Win!"));

    APlayerController *PlayerController = Cast<APlayerController>(GetController());
    GameWinWidget = CreateWidget<UUserWidget>(GetWorld(), GameWinWidgetClass);
    if (GameWinWidgetClass)
    {
        GameWinWidget->AddToViewport();
        if (PlayerController)
        {
            DisableInput(PlayerController);
            PlayerController->bShowMouseCursor = true;
        }
    }
    Protocol::C_DEFEAT Pkt;

    SEND_PACKET(Pkt);

    UNetworkManager *GameInstance = Cast<UNetworkManager>(GetGameInstance());
    GameInstance->DisconnectFromGameServer();
}

//  ۿ .  ʿ .  Դ ͵ DecreaseMental Լ ̿ϵ ġ
float ANetworkPlayer::TakeDamage(float DamageAmount, FDamageEvent const &DamageEvent,
                                 AController *EventInstigator, AActor *DamageCauser)
{
    if (bIsDead)
        return 0.0f;

    if (bIsFear && CurrentMental <= 0.0f)
    {
        GameOver();
        return 0.0f;
    }

    //
    float AppliedDamage = FMath::Clamp(DamageAmount, 0.0f, CurrentMental);
    CurrentMental -= AppliedDamage;

    // ŷ 0 Ϸ
    if (CurrentMental <= 0.0f)
    {
        CurrentMental = 0.0f; //   (ʿϸ  )
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

    if (CurrentCandleRoom)
    {
        CurrentCandleRoom->Tags.Remove(FName("Active"));
		CurrentCandleRoom->TurnOffEffects();
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
    TArray<AActor *> FoundTriggers1;
    TArray<AActor *> FoundTriggers2;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("CandleRoom"), FoundTriggers1);
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("CandleRoom2"), FoundTriggers2);

    //  ƮŰ CandleRoom 迭 CandleRoom2 迭 Ǻ
    FName CurrentMapTag;
    if (CurrentCandleRoom && CurrentCandleRoom->Tags.Contains(FName("CandleRoom")))
    {
        CurrentMapTag = FName("CandleRoom");
    }
    else if (CurrentCandleRoom && CurrentCandleRoom->Tags.Contains(FName("CandleRoom2")))
    {
        CurrentMapTag = FName("CandleRoom2");
    }
    else
    {
        // CurrentCandleRoom  ⺻ CandleRoom ó
        CurrentMapTag = FName("CandleRoom");
    }

    //   Ʈŵ ߿  ƮŰ ƴ ͸ 
    TArray<AActor *> *AllTriggers = nullptr;

    if (CurrentMapTag == FName("CandleRoom"))
    {
        AllTriggers = &FoundTriggers1;
    }
    else
    {
        AllTriggers = &FoundTriggers2;
    }

    TArray<AActor *> AvailableTriggers;
    for (AActor *Trigger : *AllTriggers)
    {
        if (Trigger && Trigger != CurrentCandleRoom)
        {
            AvailableTriggers.Add(Trigger);
        }
    }

    if (AvailableTriggers.Num() > 0)
    {
        int32 Index = FMath::RandRange(0, AvailableTriggers.Num() - 1);
        AActor *SelectedTrigger = AvailableTriggers[Index];

        if (CurrentCandleRoom)
        {
            CurrentCandleRoom->Tags.Remove(FName("Active"));
			ACandleRoom* OldCandleRoom = Cast<ACandleRoom>(CurrentCandleRoom);
			if(OldCandleRoom)
			{
				OldCandleRoom->TurnOffEffects();
			}
        }

        SelectedTrigger->Tags.AddUnique(FName("Active"));
        CurrentCandleRoom = Cast<ACandleRoom>(SelectedTrigger);

        if (ACandleRoom *CandleRoom = Cast<ACandleRoom>(SelectedTrigger))
        {
			CandleRoom->TurnOnEffects();
        }

        UE_LOG(LogTemp, Log, TEXT("Activated Mental Trigger: %s"), *SelectedTrigger->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No available triggers found in %s"), *CurrentMapTag.ToString());
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

/* 발자국 소리 이벤트*/
void ANetworkPlayer::MakeRunNoiseEvent()
{
    UE_LOG(LogTemp, Warning, TEXT("MakeRunNoiseEvent is Activate"));
    UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), RunLoudness, this, RunNoise, FName(TEXT("RunFootprint")));
    /* 1.of는 Loudness, RunNoise는 Max Range로 아무리 소리가 커도 Max Range 이후로는 전달되지 않는다.*/
}

void ANetworkPlayer::MakeWalkNoiseEvent()
{
    UE_LOG(LogTemp, Warning, TEXT("MakeWalkNoiseEvent is Activate"));
    UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), WalkLoudness, this, WalkNoise, FName(TEXT("WalkFootprint")));
    /* 1.of는 Loudness, WalkNoise는 Max Range로 아무리 소리가 커도 Max Range 이후로는 전달되지 않는다.*/
}


void ANetworkPlayer::OnCreatureAttackCamera(ACreatureBase *Creature, UTextureRenderTarget2D *RenderTarget)
{
    if (!AttackCameraWidget || !Creature || !RenderTarget)
    {
        return;
    }

    // 여기 어떤 플레이어가 화면을 봐야 하는지

    // 일단 모든 플레이어에게 보여주기
    AttackCameraWidget->ShowAttackCamera(RenderTarget, 3.0f);

    UE_LOG(LogTemp, Log, TEXT("Player Controller received attack camera from %s"), *Creature->GetName());
}

void ANetworkPlayer::SwitchCameraView(const FInputActionValue &Value)
{
    float ChangedY = -CameraBoom->SocketOffset.Y;
    StartCameraLerp(FVector(0, ChangedY, CameraBoom->SocketOffset.Z));
}

// 상황에 따라 Duration과 NewOffset 조절
// Duration이 너무 높거나 NewOffset이 SpringArm->SocketOffset+32처럼 고정값이 아니면 시점이 무너질 수 있음
void ANetworkPlayer::StartCameraLerp(const FVector &NewOffset)
{
    if (bIsLerping)
    {
        GetWorld()->GetTimerManager().ClearTimer(CameraLerpTimer);
    }

    StartOffset = CameraBoom->SocketOffset;
    EndOffset = NewOffset;
    LerpElapsed = 0.0f;
    bIsLerping = true;

    GetWorld()->GetTimerManager().SetTimer(CameraLerpTimer, this, &ANetworkPlayer::StepCameraLerp, LerpStepTime, true);
}

void ANetworkPlayer::StepCameraLerp()
{
    LerpElapsed += LerpStepTime;
    // float Alpha = FMath::Clamp(LerpElapsed / CameraLerpDuration, 0.0f, 1.0f);
    float EasedAlpha = FMath::InterpEaseInOut(0.0f, 1.0f, FMath::Clamp(LerpElapsed / CameraLerpDuration, 0.0f, 1.0f), 2.0f);

    FVector NewOffset = FMath::Lerp(StartOffset, EndOffset, EasedAlpha);
    CameraBoom->SocketOffset = NewOffset;

    if (EasedAlpha >= 1.0f)
    {
        GetWorld()->GetTimerManager().ClearTimer(CameraLerpTimer);
        bIsLerping = false;
    }
}

void ANetworkPlayer::UpdateCameraLag()
{
    if (bIsSprinting)
    {
        CameraBoom->CameraLagSpeed = 15.f;
        CameraBoom->CameraLagSpeed = 20.f;
    }
    else if (bIsCrouch)
    {
        CameraBoom->CameraLagSpeed = 6.f;
        CameraBoom->CameraLagMaxDistance = 35.f;
    }
    else
    {
        CameraBoom->CameraLagSpeed = 9.f;
        CameraBoom->CameraLagMaxDistance = 30.f;
    }
}

void ANetworkPlayer::OnForceActivatePressed()
{
    if (TouchingCandleRoom != nullptr && !TouchingCandleRoom->ActorHasTag(FName("Active")))
    {
        GetWorld()->GetTimerManager().SetTimer(
            ForceActivateTimerHandle,
            this,
            &ANetworkPlayer::ForceActivateCandleRoom,
            10.0f,
            false);
        UE_LOG(LogTemp, Log, TEXT("Force activation started."));
    }
}

void ANetworkPlayer::OnForceActivateReleased()
{
    // 버튼을 떼면 타이머 취소
    GetWorld()->GetTimerManager().ClearTimer(ForceActivateTimerHandle);
    UE_LOG(LogTemp, Log, TEXT("Force activation cancelled."));
}

void ANetworkPlayer::ForceActivateCandleRoom()
{
    GetWorld()->GetTimerManager().ClearTimer(ForceActivateTimerHandle);
    if (TouchingCandleRoom == nullptr) return;

    if (CurrentCandleRoom && CurrentCandleRoom->ActorHasTag(FName("Active")))
    {
        UE_LOG(LogTemp, Warning, TEXT("Deactivating old room: %s"), *CurrentCandleRoom->GetName());
        CurrentCandleRoom->Tags.Remove(FName("Active"));
        CurrentCandleRoom->TurnOffEffects();
    }

    CurrentCandleRoom = TouchingCandleRoom;
    UE_LOG(LogTemp, Warning, TEXT("Activating new room: %s"), *CurrentCandleRoom->GetName());
    CurrentCandleRoom->Tags.Add(FName("Active"));
    CurrentCandleRoom->TurnOnEffects();
    StartMentalRegen(10.f);
}
