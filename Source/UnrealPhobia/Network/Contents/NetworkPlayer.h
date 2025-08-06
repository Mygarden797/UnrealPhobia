// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Network/Contents/ProtoPlayer.h"
#include "InputActionValue.h"
#include "Creature/CreatureBase.h" // ũ���� ���̽� ���
#include "AttackCameraWidget.h"    // ���� ī�޶� ���� ���
#include "NetworkPlayer.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;

/*
    - Name				: ANetworkPlayer
    - Description		: Network-enabled Player Character with Survivor features
    - Date				: 2025/06/10
*/

UCLASS(Blueprintable)
class UNREALPHOBIA_API ANetworkPlayer : public AProtoPlayer
{
    GENERATED_BODY()

public:
    ANetworkPlayer(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

    // UI Components
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UUserWidget> CrosshairWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UUserWidget> GameOverWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UUserWidget> GameWinWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UInventoryWidget> InventoryWidgetClass;

    UPROPERTY()
    class UUserWidget *CrosshairWidget;

    UPROPERTY()
    class UUserWidget *GameOverWidget;

    UPROPERTY()
    class UUserWidget *GameWinWidget;

    UPROPERTY()
    class UInventoryWidget *InventoryWidget;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UStaminaBar> StaminaBarClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UMentalBar> MentalBarClass;

    UPROPERTY()
    class UStaminaBar *StaminaBar;

    UPROPERTY()
    class UMentalBar *MentalBar;

    // Movement Functions
    void Move(const FInputActionValue &Value);
    // void MoveForward(const FInputActionValue& Value);
    // void MoveRight(const FInputActionValue& Value);
    void MoveReleased();
    void Look(const FInputActionValue &Value);
    // ChangeView
    void SwitchCameraView(const FInputActionValue &Value);

    // Sprint Handler
    void Sprint(const FInputActionValue &Value);
    void StartSprint();
    void StopSprint();
    void UpdateDirectionWeight(FVector MoveDir);

    // Stamina Functions
    void LossStamina();
    void RegenStamina();

    // Crouch Function
    void SetCrouch(const FInputActionValue &value);

    // Mental Functions
    void IncreaseMental(float Amount);
    void DecreaseMental(float Amount);
    void StartMentalRegen(float Duration);
    void StopMentalRegen();

    void GameWin();

    // Attacked
    float TakeDamage(float DamageAmount, FDamageEvent const &DamageEvent,
                     AController *EventInstigator, AActor *DamageCauser);

    /** Returns CameraBoom subobject **/
    FORCEINLINE class USpringArmComponent *GetCameraBoom() const { return CameraBoom; }
    /** Returns FollowCamera subobject **/
    FORCEINLINE class UCameraComponent *GetFollowCamera() const { return FollowCamera; }

    UPROPERTY()
    AActor *CurrentTrigger;
    void DecreaseMental();
    void RegenMental();
    void ActivateRandomMentalTrigger();

    UFUNCTION()
    void OnCreatureAttackCamera(ACreatureBase *Creature, UTextureRenderTarget2D *RenderTarget);

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // Camera Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCameraComponent> FollowCamera;

    FTimerHandle CameraLerpTimer;
    FVector StartOffset;
    FVector EndOffset;
    float CameraLerpDuration = 0.1f;
    float LerpElapsed = 0.0f;
    float LerpStepTime = 0.01f;
    bool bIsLerping = false;

    // Move Camera Smoothly 
    void StartCameraLerp(const FVector& NewOffset);
    void StepCameraLerp();
    void UpdateCameraLag();

    //// Enhanced Input Components
    // UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    // class UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputMappingContext> SurvivorMovingContext;

    // Input Actions
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> LookAction;
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> SwitchViewAction;

    // UPROPERTY(EditDefaultsOnly, Category = "Input")
    // TObjectPtr<UInputAction> MoveForwardAction;

    // UPROPERTY(EditDefaultsOnly, Category = "Input")
    // TObjectPtr<UInputAction> MoveRightAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> SprintAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> CrouchAction;

    // State Variables
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float BaseSpeed = 300.0f;

    //ProtoPlayer로 이전
    //UPROPERTY(BlueprintReadWrite, Category = "Stamina")
    //bool bIsSprinting = false;

    //UPROPERTY(BlueprintReadWrite, Category = "Stamina")
    //bool bIsCrouch = false;

    UPROPERTY(BlueprintReadWrite, Category = "Mental")
    bool bIsFear = false;

    UPROPERTY(BlueprintReadWrite, Category = "Mental")
    bool bIsDead = false;

    // Mental System
    UPROPERTY(EditDefaultsOnly, Category = "Mental")
    float MaxMental = 200.f;

    UPROPERTY(VisibleAnywhere, Category = "Mental")
    float CurrentMental = 200.f;

    UPROPERTY(EditDefaultsOnly, Category = "Mental")
    float MentalDecayRate = 0.5f;

    FTimerHandle MentalDecayTimerHandle;
    FTimerHandle MentalRegenTimerHandle;
    FTimerHandle MentalRegenDurationHandle;
    bool bIsInMentalRegenZone = false;

    float MentalRegenTickTime = 1.f;
    float MentalRegenPerTick = 6.f;

    void GameOver();

    // Network-related variables
    const float MOVE_PACKET_SEND_DELAY = 0.2f;
    float MovePacketSendTimer = MOVE_PACKET_SEND_DELAY;

    // Cache for network
    FVector2D DesiredInput;
    FVector DesiredMoveDirection;
    float DesiredYaw;

    // Dirty Flag Test
    FVector2D LastDesiredInput;

    // ���� ī�޶� ���� Ŭ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UAttackCameraWidget> AttackCameraWidgetClass;

    // ���� ī�޶� ���� �ν��Ͻ�
    UPROPERTY()
    UAttackCameraWidget *AttackCameraWidget;

private:
    // Stamina System
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
    bool bIsLossingStamina = false;

    float RegenStartMental = 0.f;
    float RegenTargetAmount = 0.f;

    // UI Update Functions
    void UpdateStaminaBar();
    void UpdateMentalBar();

    /*AI Perception Hearing*/
public:
    UFUNCTION(BlueprintCallable)
    void MakeFootprintNoiseEvent();

private:
    float FootPrintNoise = 3000.f;
};