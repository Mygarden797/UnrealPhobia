#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Survivor.generated.h" // ����

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
	ASurvivor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> CrosshairWidgetClass;
	UPROPERTY()
	class UUserWidget* CrosshairWidget;

	// ĳ���� �յ� �̵�, 1D Vector
	void MoveForward(const FInputActionValue& Value);
	// ĳ���� �¿� �̵�, 1D Vector
	void MoveRight(const FInputActionValue& Value);
	// ī�޶� �̵�, 2D Vector
	void Look(const FInputActionValue& Value);

	// Sprint Handler 
	void Sprint(const FInputActionValue& Value);
	// LShift�� ������ ���� �� �޸���.
	void StartSprint();
	// LShift�� ������ ���� �� �޸��⸦ �����.
	void StopSprint();

	// ĳ���Ͱ� �޸��� �� ���׹̳ʰ� �����Ѵ�.
	void LossStamina();
	// ĳ���Ͱ� �޸��� ������ ���׹̳ʸ� ȸ���Ѵ�.
	void RegenStamina();


// ��Ż ��� �߰�(25.6.1)

	void IncreaseMental(float Amount);
	void DecreaseMental(float Amount); // �ܺ� �ǰݿ�

	// ȸ�� ����
	void StartMentalRegen(float Duration);
	void StopMentalRegen();

// ��Ż ��� �߰�(25.6.1)

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
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera");
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	FRotator TargetCameraRotation;

	// Mental 추가
	void GameOver();

	// ��Ż ��� �߰�(25.6.1)
	UPROPERTY(EditDefaultsOnly, Category = "Mental")
	float MaxMental = 200.f; // �ִ� ��Ż ����

	UPROPERTY(VisibleAnywhere, Category = "Mental")
	float CurrentMental = 200.f;

	UPROPERTY(EditDefaultsOnly, Category = "Mental")
	float MentalDecayRate = 1.f; // �ʴ� 1 ����

	FTimerHandle MentalDecayTimerHandle;

	// ��Ż ȸ����
	FTimerHandle MentalRegenTimerHandle;
	FTimerHandle MentalRegenDurationHandle;
	bool bIsInMentalRegenZone = false;

	float MentalRegenTickTime = 1.f;
	float MentalRegenPerTick = 5.f;

	UPROPERTY()
	AActor* CurrentTrigger;
	void DecreaseMental();
	void RegenMental();
	void ActivateRandomMentalTrigger();

// ��Ż ��� �߰�(25.6.1)

private:
	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	float MaxStamina = 100.f;
	UPROPERTY(VisibleAnywhere, Category = "Stamina")
	float CurrentStamina = 100.f;
	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	float StaminaLossRate = 20.f;
	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	float StaminaRegenRate = 10.f;
	UPROPERTY(EditDefaultsOnly, Category = "Stamina")

	FTimerHandle FStaminaLossHandle;
	FTimerHandle FStaminaRegenHandle;
	bool bIsLossingStamina = false;
	bool bIsSprinting = false;



};