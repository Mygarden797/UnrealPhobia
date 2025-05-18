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


/*
	- Name				: ASurvivor
	- Description		: Player Character
	- Date					: 2022/05/18, Hangyeol
	- to do					: Fix Camera Clipping when Character nearyby 'right' wall 
*/

ASurvivor::ASurvivor(const FObjectInitializer& ObjectInitializer)
{
	// Character Collision Size
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
	GetCharacterMovement()->bOrientRotationToMovement = false;					// Rotate Character depend on the Controller
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); 
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;


	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	// CameraBoom->bUsePawnControlRotation = true; 

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); 
	// FollowCamera->bUsePawnControlRotation = false; 
}

void ASurvivor::BeginPlay()
{
	Super::BeginPlay();

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