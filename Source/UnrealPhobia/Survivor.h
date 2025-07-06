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
	- Fixed					: 
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
	TSubclassOf<class UInventoryWidget> InventoryWidgetClass;

	UPROPERTY()
	class UUserWidget *CrosshairWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UStaminaBar> StaminaBarClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UMentalBar> MentalBarClass;

	UPROPERTY()
	class UStaminaBar *StaminaBar;

	UPROPERTY()
	class UMentalBar *MentalBar;

	UPROPERTY()
	class UInventoryWidget *InventoryWidget;

    // Movement Handler, 2D
	void Move(const FInputActionValue &Value);
	// Camera Movement, 2D Vector
	void Look(const FInputActionValue &Value);

	// Sprint Handler
	void Sprint(const FInputActionValue &Value);
	// LShift를 꾹 누른 상태로 달리기 시작
	void StartSprint();
	// LShift를 해제하면 달리기를 멈춤
	void StopSprint();
    // 방향에 따른 이동속도 조절 (25.07.06)
    void UpdateDirectionWeight(FVector MoveDir);

	// 스테미나 소비
	void LossStamina();
	// 스테미나 재생
	void RegenStamina();

	// ĳ���Ͱ� ��ũ����.
	void SetCrouch(const FInputActionValue &value);
		// ��Ż ��� �߰�(25.6.1)

	void IncreaseMental(float Amount);
	void DecreaseMental(float Amount);

	// ȸ�� ����
	void StartMentalRegen(float Duration);
	void StopMentalRegen();

	// ��Ż ��� �߰�(25.6.1)

	UPROPERTY()
	AActor* CurrentTrigger;
	void DecreaseMental();
	void RegenMental();
	void ActivateRandomMentalTrigger();

protected:
	virtual void BeginPlay() override;

	float CameraRotationSpeed = 50.f;

	// Enhanced Input, IMC Default
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> SurvivorMovingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

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

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category ="Movement")
    float BaseSpeed = 300.0f;

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
	float MentalRegenPerTick = 6.f;

// ��Ż ��� �߰�(25.6.1)

private:
	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	float MaxStamina = 100.f;
	UPROPERTY(VisibleAnywhere, Category = "Stamina")
	float CurrentStamina = 100.f;
	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	float StaminaLossRate = 20.f;
	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	float StaminaRegenRate = 20.f;

	FTimerHandle FStaminaLossHandle;
	FTimerHandle FStaminaRegenHandle;
	FTimerHandle FMentalTimerHandle;

	// ��Ż ��� �߰�(25.6.1)

	float RegenStartMental = 0.f;
	float RegenTargetAmount = 0.f;


	// ��Ż ��� �߰�(25.6.1)
	// ByeongJun 25.6.7
	void UpdateStaminaBar();
	void UpdateMentalBar();
	
};