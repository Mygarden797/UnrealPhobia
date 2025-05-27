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
// #include "SpringArmComponent.h"


/*
	- Name				: ASurvivor
	- Description		: Player Character
	- Date					: 2022/05/26, Hangyeol
	- Fixed					: Fix Sprint and Regen Stamina same time	
*/

ASurvivor::ASurvivor(const FObjectInitializer& ObjectInitializer)
{
	// Character Collision Size
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
	GetCharacterMovement()->bOrientRotationToMovement = false;					// Rotate Character depend on the Controller
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); 
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	CurrentStamina = MaxStamina;
	//UE_LOG(LogTemp, Error, TEXT("CurrentStamina, Init: %f"), CurrentStamina);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 90.0f;
	SpringArm->SocketOffset = FVector(10.f, 45.f, 0.f);
	SpringArm->ProbeSize = 12.f;
	// SpringArm->bUsePawnControlRotation = true; 

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName); 
	// FollowCamera->bUsePawnControlRotation = false; 

	
}

void ASurvivor::BeginPlay()
{
	Super::BeginPlay();

	CurrentStamina = MaxStamina;
	UE_LOG(LogTemp, Error, TEXT("CurrentStamina, Init: %f"), CurrentStamina);
	UE_LOG(LogTemp, Error, TEXT("CurrentStamina, Init: %f"), CurrentStamina);
	UE_LOG(LogTemp, Error, TEXT("CurrentStamina, Init: %f"), CurrentStamina);


	// Load Key Binding
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to Mapping Keys"));
	}

	// Load Crosshair
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
		UE_LOG(LogTemp, Error, TEXT("Failed to load Crosshair!!"));
	}

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
		EIC->BindAction(SprintAction, ETriggerEvent::Completed, this, & ASurvivor::Sprint);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to Bind Input!!"));
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
		UE_LOG(LogTemp, Error, TEXT("No Controller or AxisValue is 0 in MoveForward!!"));
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
		UE_LOG(LogTemp, Error, TEXT("No Controller or AxisValue is 0 in MoveRight!!"));
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
		UE_LOG(LogTemp, Error, TEXT("No Controller in Look!!"));
	}
}

void ASurvivor::Sprint(const FInputActionValue& Value)
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

void ASurvivor::StartSprint()
{
	if (bIsSprinting || CurrentStamina <= 0.f)
		return;
	bIsSprinting = true;
	GetCharacterMovement()->MaxWalkSpeed = 900.f;
	bIsLossingStamina = true;

	if (!GetWorldTimerManager().IsTimerActive(StaminaLossHandle))
	{
			GetWorldTimerManager().ClearTimer(StaminaLossHandle);
			GetWorldTimerManager().SetTimer(
			StaminaLossHandle,
			this,
			&ASurvivor::LossStamina,
			0.1f,
			true
		);
	}
	UE_LOG(LogTemp, Display, TEXT("CurrentStamina, Start: %f"), CurrentStamina);
}

void ASurvivor::StopSprint()
{
	bIsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = 400.f;

	bIsLossingStamina = false;
	GetWorldTimerManager().ClearTimer(StaminaLossHandle);

	if (CurrentStamina < MaxStamina)
	{
		GetWorldTimerManager().SetTimer(StaminaRegenHandle, this, &ASurvivor::RegenStamina, 1.0f, true);	
	}
	UE_LOG(LogTemp, Display, TEXT("CurrentStamina, Stop: %f"), CurrentStamina);
}

void ASurvivor::LossStamina()
{
	if (!bIsLossingStamina || GetVelocity().SizeSquared() <= 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("LossStamina: !bIsLossingStamina is False"));
		return;
	}

	const float Delta = StaminaLossRate * 0.1f;
	if (CurrentStamina <= Delta)
	{
		CurrentStamina = 0.f;
		UE_LOG(LogTemp, Display, TEXT("LossStamina() - Sprint Á¾·á, Stamina: %.2f"), CurrentStamina);
		StopSprint(); 
	}

	else
	{
		CurrentStamina -= Delta;
		UE_LOG(LogTemp, Display, TEXT("CurrentStamina, Loss: %f"), CurrentStamina);
	}
}

void ASurvivor::RegenStamina()
{
	if (bIsSprinting || CurrentStamina >= MaxStamina)
	{
		GetWorldTimerManager().ClearTimer(StaminaRegenHandle);
		return;
	}
	else {
		CurrentStamina += StaminaRegenRate * 0.1f;
		UE_LOG(LogTemp, Display, TEXT("CurrentStamina, Regen: %f"), CurrentStamina);
		CurrentStamina = FMath::Min(CurrentStamina, MaxStamina);
	}
}