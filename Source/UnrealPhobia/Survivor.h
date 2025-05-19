#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Survivor.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;

UCLASS(Blueprintable)
class UNREALPHOBIA_API ASurvivor : public ACharacter
{
	GENERATED_BODY()

public:
	ASurvivor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> CrosshairWidgetClass;
	UPROPERTY()
	class UUserWidget* CrosshairWidget;

private:

	// Move Front and Back using 1D Vector, Keyboard
	void MoveForward(const FInputActionValue& Value);
	// Move Right and Left using 1D Vector, Keyboard
	void MoveRight(const FInputActionValue& Value);
	// Move Camera using 2D Vector, Mouse
	void Look(const FInputActionValue& Value);


protected:
	virtual void BeginPlay() override;

	float CameraRotationSpeed = 50.f;

	// Enhanced Input, IMC Default
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveForwardAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveRightAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera");
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;


	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	FRotator TargetCameraRotation;

};