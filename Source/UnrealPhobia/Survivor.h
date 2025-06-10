#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Survivor.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;

/*
	- Name				: ASurvivor
	- Description		: Player Character
	- Date					: 2022/05/26, Hangyeol
	- Fixed					: 스프린트 중에 스테미너가 회복되는 현상
*/

UCLASS(Blueprintable)
class UNREALPHOBIA_API ASurvivor : public ACharacter
{
	GENERATED_BODY()

public:
	ASurvivor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, 
	//	class AController* EventIstigator, AActor* DamageCauser) override;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> CrosshairWidgetClass;
	UPROPERTY()
	class UUserWidget* CrosshairWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UStaminaBar> StaminaBarClass;
	UPROPERTY()
	class UStaminaBar* StaminaBar;

	// 캐릭터 앞뒤 이동, 1D Vector
	void MoveForward(const FInputActionValue& Value);
	// 캐릭터 좌우 이동, 1D Vector
	void MoveRight(const FInputActionValue& Value);
	// 카메라 이동, 2D Vector
	void Look(const FInputActionValue& Value);

	// Sprint Handler 
	void Sprint(const FInputActionValue& Value);
	// LShift를 누르고 있을 때 달린다.
	void StartSprint();
	// LShift를 누르지 않을 때 달리기를 멈춘다.
	void StopSprint();

	// 캐릭터가 달리는 중 스테미너가 감소한다.
	void LossStamina();
	// 캐릭터가 달리지 않으면 스테미너를 회복한다.
	void RegenStamina();

	// 캐릭터가 웅크린다.
	void SetCrouch(const FInputActionValue& value);

protected:
	virtual void BeginPlay() override;

	float CameraRotationSpeed = 50.f;

	// Enhanced Input, IMC Default
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> SurvivorMovingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveForwardAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveRightAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> CrouchAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera");
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(BlueprintReadWrite, Category = "Stamina")
	bool bIsSprinting = false;
	UPROPERTY(BlueprintReadWrite, Category = "Stamina")
	bool bIsCrouch = false;

	UPROPERTY(BlueprintReadWrite, Category = "Mental")
	bool bIsFear = false;
	UPROPERTY(BlueprintReadWrite, Category = "Mental")
	bool bIsDead = false;

	FRotator TargetCameraRotation;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	float MaxStamina = 100.f;
	UPROPERTY(VisibleAnywhere, Category = "Stamina")
	float CurrentStamina = 100.f;
	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	float StaminaLossRate = 20.f;
	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	float StaminaRegenRate = 20.f;

	UPROPERTY(EditAnywhere, Category = "Mental")
	float MaxMental = 100.0f;
	UPROPERTY(VisibleAnywhere, Category = "Mental")
	float CurrentMental = 100.0f;

	FTimerHandle FStaminaLossHandle;
	FTimerHandle FStaminaRegenHandle;
	FTimerHandle FMentalTimerHandle;
	
};