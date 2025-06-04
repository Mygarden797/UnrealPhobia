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

ASurvivor::ASurvivor(const FObjectInitializer& ObjectInitializer)
{
	// 캐릭터 충돌 크기
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
	GetCharacterMovement()->bOrientRotationToMovement = false;					// 컨트롤러 기준으로 캐릭터 회전
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); 
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 200.f;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	CurrentStamina = MaxStamina;
	// UE_LOG(LogTemp, Error, TEXT("CurrentStamina, Init: %f"), CurrentStamina);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 90.0f;
	SpringArm->SocketOffset = FVector(10.f, 45.f, 0.f);
	SpringArm->ProbeSize = 12.f;
	// SpringArm->bUsePawnControlRotation = true; 

	// SpringArm 컴포넌트 생성
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

	// 키 바인딩 가져오기
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(SurvivorMovingContext, 0);
		}
	}
	else
	{
		// UE_LOG(LogTemp, Error, TEXT("Failed to Mapping Keys"));
	}

	// 조준선 가져오기
	if (IsLocallyControlled() && CrosshairWidgetClass)
	{
		CrosshairWidget =CreateWidget<UUserWidget>(GetWorld(), CrosshairWidgetClass);
		if (CrosshairWidget) 
		{
			CrosshairWidget->AddToViewport();
		}
	}
	else
	{
		// UE_LOG(LogTemp, Error, TEXT("Failed to load Crosshair!!"));
	}

	// 스테미너 UI 가져오기
	if (IsLocallyControlled() && StaminaBarClass)
	{
		StaminaBar = CreateWidget<UStaminaBar>(GetWorld(), StaminaBarClass);
		if (StaminaBar)
		{
			StaminaBar->AddToViewport();
		}
	}


	GetWorld()->GetTimerManager().SetTimer(FMentalTimerHandle, this, &ASurvivor::LossMental, 1.0f, true);

}

void ASurvivor::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
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

void ASurvivor::MoveForward(const FInputActionValue& Value)
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

void ASurvivor::MoveRight(const FInputActionValue& Value)
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

void ASurvivor::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y*-1);
	}
	else
	{
		// UE_LOG(LogTemp, Error, TEXT("No Controller in Look!!"));
	}
}

void ASurvivor::Sprint(const FInputActionValue& Value)
{
	const bool DoSprint = Value.Get<bool>();			// LShift를 누르고 있을 때 true

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
	if (bIsSprinting || CurrentStamina <= 0.f)			// 달리기 중이거나 스테미너 없으면 리턴
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
			true
		);
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
		// UE_LOG(LogTemp, Display, TEXT("LossStamina() - Sprint 종료, Stamina: %.2f"), CurrentStamina);
		StopSprint(); 
	}
	else
	{
		CurrentStamina -= StaminaLossRate * 0.1f;
		// UE_LOG(LogTemp, Display, TEXT("CurrentStamina, Loss: %f"), CurrentStamina);
	}
	if (StaminaBar)
	{
		float Percent = CurrentStamina / MaxStamina;
		StaminaBar->SetStaminaPercent(Percent);
	}
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

	if (StaminaBar)
	{
		float Percent = CurrentStamina / MaxStamina;
		StaminaBar->SetStaminaPercent(Percent);
	}

}

void ASurvivor::SetCrouch(const FInputActionValue& value)
{
	const bool bPressed = value.Get<bool>();
	if (bPressed)
	{
		Crouch();
	}
	else
	{
		UnCrouch();
	}
}

void ASurvivor::LossMental()
{
	if (bIsDead) return;

	if (CurrentMental > 0.0f)
	{
		CurrentMental = FMath::Max(CurrentMental - 1.0f, 0.0f);
		bIsFear = false;
	}

	if (CurrentMental <= 0.0f && !bIsFear)
	{
		bIsFear = true;
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