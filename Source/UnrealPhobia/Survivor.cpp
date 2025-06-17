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

#include "StaminaBar.h"
#include "Kismet/GameplayStatics.h"

#include "MentalBar.h"
#include "InventoryWidget.h"
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
		1.0f, // �ʴ� 1ȸ
		true);
	
	// Deactivates all CandleRooms
	// TArray<AActor*> FoundTriggers;
	// UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("CandleRoom"), FoundTriggers);

	// for (AActor* Trigger : FoundTriggers)
	// {
	// 	Trigger->Tags.Remove(FName("Active")); // 태그 제거 → 트리거 내부에서 Active 기준으로 동작하게 만들 것
	// }
	// ActivateRandomMentalTrigger();

	TArray<AActor*> FoundTriggers;
	TArray<AActor*> FoundTriggers2;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("CandleRoom"), FoundTriggers);
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("CandleRoom2"), FoundTriggers2);

	TArray<AActor*> AvailableTriggers;
	for (AActor* Trigger : FoundTriggers)
	{
		if (Trigger != CurrentTrigger)
		{
			AvailableTriggers.Add(Trigger);
		}
	}

	TArray<AActor*> AvailableTriggers2;
	for (AActor* Trigger : FoundTriggers2)
	{
		if (Trigger != CurrentTrigger)
		{
			AvailableTriggers2.Add(Trigger);
		}
	}

// CandleRoom 트리거 이름 출력
UE_LOG(LogTemp, Log, TEXT("=== FoundTriggers (CandleRoom) ==="));
for (AActor* Trigger : FoundTriggers)
{
    if (Trigger)
    {
        UE_LOG(LogTemp, Log, TEXT("Trigger Name: %s"), *Trigger->GetName());
    }
}

// CandleRoom2 트리거 이름 출력
UE_LOG(LogTemp, Log, TEXT("=== FoundTriggers2 (CandleRoom2) ==="));
for (AActor* Trigger : FoundTriggers2)
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
		AActor* SelectedTrigger = AvailableTriggers[Index];
		AActor* SelectedTrigger2 = AvailableTriggers2[Index2];
		SelectedTrigger->Tags.AddUnique(FName("Active"));  // Activate with Active Tag
		SelectedTrigger2->Tags.AddUnique(FName("Active"));

		UE_LOG(LogTemp, Log, TEXT("Activated Mental Trigger: %s"), *SelectedTrigger->GetName());
		UE_LOG(LogTemp, Log, TEXT("Activated Mental Trigger: %s"), *SelectedTrigger2->GetName());
	}

// ��Ż����߰�(25.6.1)
	// ���׹̳� UI ��������
	CurrentStamina = MaxStamina;
	if (IsLocallyControlled() && StaminaBarClass)
	{
		StaminaBar = CreateWidget<UStaminaBar>(GetWorld(), StaminaBarClass);
		if (StaminaBar)
		{
			StaminaBar->AddToViewport();
		}
	}

	CurrentMental = MaxMental;
	if (IsLocallyControlled() && MentalBarClass)
	{
		MentalBar = CreateWidget<UMentalBar>(GetWorld(), MentalBarClass);
		if (MentalBar)
		{
			MentalBar->AddToViewport();
		}
	}

	if (IsLocallyControlled() && InventoryWidgetClass)
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

void ASurvivor::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
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
	if (bIsCrouch)
		return;
	if (bIsSprinting || CurrentStamina <= 0.f)			// �޸��� ���̰ų� ���׹̳� ������ ����
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

	// 1. 캐릭터 입력 비활성화
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		DisableInput(PlayerController);
	}

	// 2. 화면 페이드아웃 (블랙)
	if (PlayerController && PlayerController->PlayerCameraManager)
	{
		// Params: FromAlpha, ToAlpha, Duration, Color, bShouldFadeAudio, bHoldWhenFinished
		PlayerController->PlayerCameraManager->StartCameraFade(
			0.f,            // FromAlpha (투명)
			1.f,            // ToAlpha (불투명)
			1.f,            // Duration (2초 동안 페이드)
			FLinearColor::Black, // Color
			false,          // bShouldFadeAudio
			true            // bHoldWhenFinished
		);
	}
}

// Mental
void ASurvivor::StartMentalRegen(float Duration)
{
	bIsInMentalRegenZone = true;

	GetWorldTimerManager().SetTimer(MentalRegenTimerHandle, this, &ASurvivor::RegenMental, MentalRegenTickTime, true);
	GetWorldTimerManager().SetTimer(MentalRegenDurationHandle, this, &ASurvivor::StopMentalRegen, Duration, false);
}

void ASurvivor::StopMentalRegen()
{
	bIsInMentalRegenZone = false;
	GetWorldTimerManager().ClearTimer(MentalRegenTimerHandle);
	GetWorldTimerManager().ClearTimer(MentalRegenDurationHandle);
	CurrentTrigger->Tags.Remove(FName("Active"));

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

void ASurvivor::ActivateRandomMentalTrigger()
{
	TArray<AActor*> FoundTriggers1;
	TArray<AActor*> FoundTriggers2;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("CandleRoom"), FoundTriggers1);
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("CandleRoom2"), FoundTriggers2);

	// 현재 트리거가 CandleRoom 계열인지 CandleRoom2 계열인지 판별
	FName CurrentMapTag;
	if (CurrentTrigger && CurrentTrigger->Tags.Contains(FName("CandleRoom")))
	{
		CurrentMapTag = FName("CandleRoom");
	}
	else if (CurrentTrigger && CurrentTrigger->Tags.Contains(FName("CandleRoom2")))
	{
		CurrentMapTag = FName("CandleRoom2");
	}
	else
	{
		// CurrentTrigger가 없으면 기본 CandleRoom으로 처리
		CurrentMapTag = FName("CandleRoom");
	}

	// 같은 맵의 트리거들 중에서 현재 트리거가 아닌 것만 고르기
	TArray<AActor*>* AllTriggers = nullptr;

	if (CurrentMapTag == FName("CandleRoom"))
	{
		AllTriggers = &FoundTriggers1;
	}
	else
	{
		AllTriggers = &FoundTriggers2;
	}

	TArray<AActor*> AvailableTriggers;
	for (AActor* Trigger : *AllTriggers)
	{
		if (Trigger && Trigger != CurrentTrigger)
		{
			AvailableTriggers.Add(Trigger);
		}
	}

	if (AvailableTriggers.Num() > 0)
	{
		int32 Index = FMath::RandRange(0, AvailableTriggers.Num() - 1);
		AActor* SelectedTrigger = AvailableTriggers[Index];

		if (CurrentTrigger)
		{
			CurrentTrigger->Tags.Remove(FName("Active"));
		}

		SelectedTrigger->Tags.AddUnique(FName("Active"));
		CurrentTrigger = SelectedTrigger;

		UE_LOG(LogTemp, Log, TEXT("Activated Mental Trigger: %s"), *SelectedTrigger->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No available triggers found in %s"), *CurrentMapTag.ToString());
	}
}

<<<<<<< Updated upstream
=======


>>>>>>> Stashed changes
/*

float ASurvivor::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDead) return 0.0f;

	if (bIsFear && CurrentMental <= 0.0f)
	{
		Die();
		return DamageAmount;
	}

	float AppliedDamage = FMath::Min(CurrentMental, DamageAmount);
	CurrentMental -= AppliedDamage;

	if (CurrentMental < 0.0f)
	{
		bIsFear = true;
	}
}

void ASurvivor::Die()
{
	bIsDead = true;
	// bIsFear = false;

	GetWorld()->GetTimerManager().ClearTimer(FMentalTimerHandle);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetSimulatePhysics(true);
	DisableInput(nullptr);
}
*/



void ASurvivor::SetCrouch(const FInputActionValue &value)
{
	if (bIsSprinting)
	{
		bIsSprinting = false;
	}
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

// Update Stamina Bar UI
void ASurvivor::UpdateStaminaBar()
{
	if (StaminaBar)
	{
		float Percent = CurrentStamina / MaxStamina;
		StaminaBar->SetStaminaPercent(Percent);
	}
}

// Update Mental Bar UI
void ASurvivor::UpdateMentalBar()
{
	if (MentalBar)
	{
		float Percent = CurrentMental / MaxMental;
		MentalBar->SetMentalPercent(Percent);
	}
}

/*

float ASurvivor::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDead) return 0.0f;

	if (bIsFear && CurrentMental <= 0.0f)
	{
		Die();
		return DamageAmount;
	}

	float AppliedDamage = FMath::Min(CurrentMental, DamageAmount);
	CurrentMental -= AppliedDamage;

	if (CurrentMental < 0.0f)
	{
		bIsFear = true;
	}
}

void ASurvivor::Die()
{
	bIsDead = true;
	// bIsFear = false;

	GetWorld()->GetTimerManager().ClearTimer(FMentalTimerHandle);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetSimulatePhysics(true);
	DisableInput(nullptr);
}
*/
