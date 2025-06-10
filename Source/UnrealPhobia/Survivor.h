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
	- Fixed					: ������Ʈ �߿� ���׹̳ʰ� ȸ���Ǵ� ����
*/

UCLASS(Blueprintable)
class UNREALPHOBIA_API ASurvivor : public ACharacter
{
	GENERATED_BODY()

public:
	ASurvivor(const FObjectInitializer &ObjectInitializer = FObjectInitializer::Get());

	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;
	// virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	//	class AController* EventIstigator, AActor* DamageCauser) override;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> CrosshairWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> InventoryWidgetClass;

	UPROPERTY()
	class UUserWidget *CrosshairWidget;

	UPROPERTY()
	class UUserWidget *InventoryWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UStaminaBar> StaminaBarClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UMentalBar> MentalBarClass;

	UPROPERTY()
	class UStaminaBar *StaminaBar;

	UPROPERTY()
	class UMentalBar *MentalBar;

	// ĳ���� �յ� �̵�, 1D Vector
	void MoveForward(const FInputActionValue &Value);
	// ĳ���� �¿� �̵�, 1D Vector
	void MoveRight(const FInputActionValue &Value);
	// ī�޶� �̵�, 2D Vector
	void Look(const FInputActionValue &Value);

	// Sprint Handler
	void Sprint(const FInputActionValue &Value);
	// LShift�� ������ ���� �� �޸���.
	void StartSprint();
	// LShift�� ������ ���� �� �޸��⸦ �����.
	void StopSprint();

	// ĳ���Ͱ� �޸��� �� ���׹̳ʰ� �����Ѵ�.
	void LossStamina();
	// ĳ���Ͱ� �޸��� ������ ���׹̳ʸ� ȸ���Ѵ�.
	void RegenStamina();

	// ĳ���Ͱ� ��ũ����.
	void SetCrouch(const FInputActionValue &value);

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

	// Mental 추가
	void GameOver();

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