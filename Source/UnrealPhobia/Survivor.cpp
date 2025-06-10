#include "Survivor.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"

ASurvivor::ASurvivor(const FObjectInitializer &ObjectInitializer)
{
	// ĳ���� �浹 ũ��
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	GetCharacterMovement()->bOrientRotationToMovement = false; // ��Ʈ�ѷ� �������� ĳ���� ȸ��
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 200.f;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	// CurrentStamina = MaxStamina;
	//  UE_LOG(LogTemp, Error, TEXT("CurrentStamina, Init: %f"), CurrentStamina);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 90.0f;
	SpringArm->SocketOffset = FVector(10.f, 45.f, 0.f);
	SpringArm->ProbeSize = 12.f;
	// SpringArm->bUsePawnControlRotation = true;

	// SpringArm ������Ʈ ����
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	// FollowCamera->bUsePawnControlRotation = false;
}

void ASurvivor::BeginPlay()
{
	Super::BeginPlay();

	CurrentStamina = MaxStamina;
	CurrentMental = MaxMental;
	// UE_LOG(LogTemp, Error, TEXT("CurrentStamina, Init: %f"), CurrentStamina);
	// UE_LOG(LogTemp, Error, TEXT("CurrentMental, Init: %f"), CurrentMental);

	// Ű ���ε� ��������
	if (APlayerController *PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem *Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(SurvivorMovingContext, 0);
		}
	}
	else
	{
		// UE_LOG(LogTemp, Error, TEXT("Failed to Mapping Keys"));
	}

	// ���ؼ� ��������
	if (IsLocallyControlled() && CrosshairWidgetClass)
	{
		CrosshairWidget = CreateWidget<UUserWidget>(GetWorld(), CrosshairWidgetClass);
		if (CrosshairWidget)
		{
			CrosshairWidget->AddToViewport();
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load Crosshair!!"));
	}

	// ��Ż����߰�(25.6.1)

	// ��Ż �ڿ� ���� Ÿ�̸�
	GetWorldTimerManager().SetTimer(
		MentalDecayTimerHandle,
		this,
		&ASurvivor::DecreaseMental,
		1.0f,  // �ʴ� 1ȸ
		true
	);
// ��Ż����߰�(25.6.1)
}

void ASurvivor::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent *EIC = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EIC->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &ASurvivor::MoveForward);
		EIC->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &ASurvivor::MoveRight);
		EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASurvivor::Look);

		EIC->BindAction(SprintAction, ETriggerEvent::Started, this, &ASurvivor::Sprint);
		EIC->BindAction(SprintAction, ETriggerEvent::Completed, this, &ASurvivor::Sprint);

		EIC->BindAction(CrouchAction, ETriggerEvent::Started, this, &ASurvivor::SetCrouch);
		EIC->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ASurvivor::SetCrouch);
	}
	else
	{
		// UE_LOG(LogTemp, Error, TEXT("Failed to Bind Input!!"));
	}
}

void ASurvivor::MoveForward(const FInputActionValue &Value)
{
	const float AxisValue = Value.Get<float>();
	if (Controller && AxisValue != 0.0f)
	{
		const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, AxisValue);

		// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Move Forward"));
	}
	else
	{
		// UE_LOG(LogTemp, Error, TEXT("No Controller or AxisValue is 0 in MoveForward!!"));
	}
}

void ASurvivor::MoveRight(const FInputActionValue &Value)
{
	const float AxisValue = Value.Get<float>();

	if (Controller && AxisValue != 0.0f)
	{
		const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, AxisValue);

		// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Move Right"));
	}
	else
	{
		// UE_LOG(LogTemp, Error, TEXT("No Controller or AxisValue is 0 in MoveRight!!"));
	}
}

void ASurvivor::Look(const FInputActionValue &Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y * -1);
	}
	else
	{
		// UE_LOG(LogTemp, Error, TEXT("No Controller in Look!!"));
	}
}

void ASurvivor::Sprint(const FInputActionValue &Value)
{
	const bool DoSprint = Value.Get<bool>(); // LShift�� ������ ���� �� true

	if (DoSprint && CurrentStamina > 0.f)
	{
		StartSprint();
	}
	else
	{
		StopSprint();
	}
}

void ASurvivor::StartSprint()
{
	if (bIsSprinting || CurrentStamina <= 0.f) // �޸��� ���̰ų� ���׹̳� ������ ����
		return;

	bIsSprinting = true;
	GetCharacterMovement()->MaxWalkSpeed = 900.f;

	if (!GetWorldTimerManager().IsTimerActive(FStaminaLossHandle))
	{
		GetWorldTimerManager().ClearTimer(FStaminaLossHandle);
		GetWorldTimerManager().SetTimer(
			FStaminaLossHandle,
			this,
			&ASurvivor::LossStamina,
			0.1f,
			true);
	}
	// UE_LOG(LogTemp, Display, TEXT("CurrentStamina, Start: %f"), CurrentStamina);
}

void ASurvivor::StopSprint()
{
	bIsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = 400.f;

	// bIsLossingStamina = false;
	GetWorldTimerManager().ClearTimer(FStaminaLossHandle);

	if (CurrentStamina < MaxStamina)
	{
		GetWorldTimerManager().SetTimer(
			FStaminaRegenHandle,
			this,
			&ASurvivor::RegenStamina,
			0.1f,
			true);
	}
	// UE_LOG(LogTemp, Display, TEXT("CurrentStamina, Stop: %f"), CurrentStamina);
}

void ASurvivor::LossStamina()
{
	if (!bIsSprinting || GetVelocity().SizeSquared() <= 0.0f)
	{
		// UE_LOG(LogTemp, Warning, TEXT("LossStamina: !bIsLossingStamina is False"));
		return;
	}

	if (CurrentStamina <= StaminaLossRate * 0.1f)
	{
		CurrentStamina = 0.0f;
		// UE_LOG(LogTemp, Display, TEXT("LossStamina() - Sprint ����, Stamina: %.2f"), CurrentStamina);
		StopSprint();
	}
	else
	{
		CurrentStamina -= StaminaLossRate * 0.1f;
		// UE_LOG(LogTemp, Display, TEXT("CurrentStamina, Loss: %f"), CurrentStamina);
	}
	UpdateStaminaBar();
}

void ASurvivor::RegenStamina()
{
	if (bIsSprinting || CurrentStamina >= MaxStamina)
	{
		GetWorldTimerManager().ClearTimer(FStaminaRegenHandle);
		return;
	}
	else
	{
		CurrentStamina += StaminaRegenRate * 0.1f;
		// UE_LOG(LogTemp, Display, TEXT("CurrentStamina, Regen: %f"), CurrentStamina);
		CurrentStamina = FMath::Min(CurrentStamina, MaxStamina);
	}

	UpdateStaminaBar();
}

// Mental(25.6.1.)

void ASurvivor::IncreaseMental(float Amount)
{
	CurrentMental = FMath::Clamp(CurrentMental + Amount, 0.f, MaxMental);
	UpdateMentalBar();
}

void ASurvivor::DecreaseMental(float Amount)
{
	CurrentMental = FMath::Clamp(CurrentMental - Amount, 0.f, MaxMental);
	UpdateMentalBar();
}

// Mental
void ASurvivor::DecreaseMental()
{
	if (CurrentMental > 0.f)
	{
		CurrentMental -= MentalDecayRate;
		CurrentMental = FMath::Clamp(CurrentMental, 0.f, MaxMental);
		UE_LOG(LogTemp, Log, TEXT("CurrentMental: %f"), CurrentMental);

		if (CurrentMental <= 0.f)
		{
			GameOver();
		}
	}
	UpdateMentalBar();
}

void ASurvivor::GameOver()
{
	UE_LOG(LogTemp, Warning, TEXT("Game Over! Mental is Zero."));
}

// Mental
void ASurvivor::StartMentalRegen(float Duration)
{
	if (bIsInMentalRegenZone) return;

	bIsInMentalRegenZone = true;

	GetWorldTimerManager().SetTimer(
		MentalRegenTimerHandle,
		this,
		&ASurvivor::RegenMental,
		RegenInterval,
		true
	);
}

void ASurvivor::StopMentalRegen()
{
	bIsInMentalRegenZone = false;
	GetWorldTimerManager().ClearTimer(MentalRegenTimerHandle);
	GetWorldTimerManager().ClearTimer(MentalRegenDurationHandle);

	ActivateRandomMentalTrigger();
}

void ASurvivor::RegenMental()
{
	if (CurrentMental >= MaxMental)
	{
		StopMentalRegen();
		return;
	}

	// if (CurrentMental - RegenStartMental >= RegenTargetAmount)
	// {
	// 	StopMentalRegen();
	// 	return;
	// }

	CurrentMental = FMath::Clamp(CurrentMental + MentalRegenPerTick, 0.f, MaxMental);
}