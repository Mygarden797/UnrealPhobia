// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Network/Contents/ProtoPlayer.h"
#include "InputActionValue.h"
#include "Creature/CreatureBase.h"        // Creature Perception
#include "AttackCameraWidget.h"          // 공격 카메라
#include "Types/ChaseSystemTypes.h"
#include "Assets/AudioAssets.h"
#include "Managers/SoundManager.h"
#include "Settings/GameSettingsWidget.h"
#include "NetworkPlayer.generated.h"
 
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class USpotLightComponent;
class UAudioAssets;

/**
 *      Name				    : ANetworkPlayer
 *      Description		    : Network-enabled Player Character with Survivor features
 *      LastUpdate		    : 2025/08/09
 *      개선사항             : 위젯 생성을 템플릿으로 일반화
*/

UCLASS(Blueprintable)
class UNREALPHOBIA_API ANetworkPlayer : public AProtoPlayer
{
    GENERATED_BODY()

public:
    ANetworkPlayer(const FObjectInitializer &ObjectInitializer = FObjectInitializer::Get());

    virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;
    // void ReinitializeInputBindings();

    // UI Components
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UUserWidget> CrosshairWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UUserWidget> GameOverWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UUserWidget> GameWinWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UInventoryWidget> InventoryWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UUserWidget> ForceActivateProgressWidgetClass;
    
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UGameSettingsWidget> AudioSettingsWidgetClass;

    UPROPERTY()
    class UUserWidget *CrosshairWidget;

    UPROPERTY()
    class UUserWidget *GameOverWidget;

    UPROPERTY()
    class UUserWidget *GameWinWidget;

    UPROPERTY()
    class UInventoryWidget *InventoryWidget;

    // 현재 보는 설정창, 당장은 오디오 창뿐임
    UPROPERTY()
    TObjectPtr<UGameSettingsWidget> CurrentSettingsWidget;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UStaminaBar> StaminaBarClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UMentalBar> MentalBarClass;

    UPROPERTY()
    class UStaminaBar *StaminaBar;

    UPROPERTY()
    class UMentalBar *MentalBar;

    UPROPERTY()
    class UUserWidget *ForceActivateProgressWidget; // Changed to UUserWidget

    /* Movement Functions */ 
    void Move(const FInputActionValue &Value);
    void MoveReleased();
    void Look(const FInputActionValue &Value);
    // ChangeView
    void SwitchCameraView(const FInputActionValue &Value);

    /* Sprint Handler */ 
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

    // Flash Function
    void SwitchFlash(const FInputActionValue &Value);
    void GameWin();

    // Attacked
    float TakeDamage(float DamageAmount, FDamageEvent const &DamageEvent,
                     AController *EventInstigator, AActor *DamageCauser);

    /** Returns CameraBoom subobject **/
    FORCEINLINE class USpringArmComponent *GetCameraBoom() const { return CameraBoom; }
    /** Returns FollowCamera subobject **/
    FORCEINLINE class UCameraComponent *GetFollowCamera() const { return FollowCamera; }

    UPROPERTY()
    class ACandleRoom* CurrentCandleRoom;
    class ACandleRoom* TouchingCandleRoom;

    void DecreaseMental();
    void RegenMental();
    void ActivateRandomMentalTrigger();

	void OnForceActivatePressed();

	void OnForceActivateReleased();

	void ForceActivateCandleRoom();

	FTimerHandle ForceActivateTimerHandle;

    // New: Force Activate Progress UI
    FTimerHandle ForceActivateProgressTimerHandle; // Timer for UI updates
    float ForceActivateProgressElapsed = 0.0f; // Elapsed time for progress
    const float ForceActivateDuration = 10.0f; // Total duration for force activation
    void UpdateForceActivateProgress(); // Function to update the UI
    void HideForceActivateProgress(); // Function to hide the UI

    UFUNCTION()
    //void OnCreatureAttackCamera(ACreatureBase *Creature, UTextureRenderTarget2D *RenderTarget, AProtoPlayer *AttackedPlayer);
    void CreatureAttackCamera(ACreatureBase* Creature);


    // ChaseState
    UFUNCTION(BlueprintCallable, Category = "Creature")
    void AddChaser();
    UFUNCTION(BlueprintCallable, Category = "Creature")
    void RemoveChaser();
    void SetChaseState(EChaseState NewState);

    /* Environment Settings Widgets */
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ToggleAudioSettings(const FInputActionValue& Value);

    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowSettings(TSubclassOf<UGameSettingsWidget> WidgetClassToShow);

    UFUNCTION(BlueprintCallable, Category = "UI")
    void HideSettings();


protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    /* Camera Components */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCameraComponent> FollowCamera;

    USpotLightComponent *FlashLight;

    UPROPERTY(EditAnywhere, Category = "Audio")
    TSubclassOf<UAudioAssets> AudioDataAssetClass;

    FTimerHandle CameraLerpTimer;
    FVector StartOffset;
    FVector EndOffset;
    float CameraLerpDuration = 0.1f;
    float LerpElapsed = 0.0f;
    float LerpStepTime = 0.01f;
    bool bIsLerping = false;

    // Move Camera Smoothly
    void StartCameraLerp(const FVector &NewOffset);
    void StepCameraLerp();
    void UpdateCameraLag();

    /* Input Mapping */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputMappingContext> UIMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputMappingContext> SurvivorMovingContext;
    // Input Actions
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> LookAction;
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> SwitchViewAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> SprintAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ForceActivateAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> SwitchFlashAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> InteractUIAction;

    bool bIsFlashing = true;

    // State Variables
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float BaseSpeed = 400.0f;

    //ProtoPlayer로 이전
    //UPROPERTY(BlueprintReadWrite, Category = "Stamina")
    //bool bIsSprinting = false;

    //UPROPERTY(BlueprintReadWrite, Category = "Stamina")
    //bool bIsCrouch = false;

    UPROPERTY(BlueprintReadWrite, Category = "Mental")
    bool bIsFear = false;

    UPROPERTY(BlueprintReadWrite, Category = "Mental")
    bool bIsDead = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Creature")
    EChaseState CurrentChaseState;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Creature")
    int32 ChaserCount = 0;
    FTimerHandle ChaseCooldownTimerHandle;
    void ReturnToSafe();

    UPROPERTY(EditAnywhere, Category = "Debug")
    bool bShowChaserCountDebug = false;
    void DisplayChaserNumberDebug();

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
    int ForceActiveCount = 3;

    void GameOver();

    /* Network */
    // Network-related variables
    const float MOVE_PACKET_SEND_DELAY = 0.2f;
    float MovePacketSendTimer = MOVE_PACKET_SEND_DELAY;

    // Cache for network
    FVector2D DesiredInput;
    FVector DesiredMoveDirection;
    float DesiredYaw;
    float DesiredSpeed;

    // Dirty Flag Test
    FVector2D LastDesiredInput;

    // ���� ī�޶� ���� Ŭ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UAttackCameraWidget> AttackCameraWidgetClass;

    // ���� ī�޶� ���� �ν��Ͻ�
    UPROPERTY()
    UAttackCameraWidget *AttackCameraWidget;

private:
    /* Stamina System */
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

    /* UI Var and Functions */
    bool bIsAudioSettingsVisible;
    void UpdateStaminaBar();
    void UpdateMentalBar();
    //void OnToggleAudioSettings(const FInputActionValue& Value);
    void CreateAudioSettingsWidget();

    /*AI Perception Hearing*/
public:
    UFUNCTION(BlueprintCallable)
    void MakeRunNoiseEvent();

    UFUNCTION(BlueprintCallable)
    void MakeWalkNoiseEvent();


    
private:
    float RunNoise = 3000.f;
    float RunLoudness = 1.3f;

    float WalkNoise = 750.f;
    float WalkLoudness = 1.0f;

    USoundManager* GetSoundManager() const;

    /*무적 시간 관리를 위한 변수*/
    float InvincibleTime = 5.0f;
    bool bIsinvincible = false;
    FTimerHandle FInvincibleTimerHandle;

public:
    bool GetIsinvincible() { return bIsinvincible;};
    void StartInvincible();
    void EndInvincible();
};