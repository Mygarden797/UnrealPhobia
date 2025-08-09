#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Settings/GameAudioSettingsWidget.h"
#include "NetworkPlayer.h"
#include "SurvivorController.generated.h"

/**
*       Name				        : SurvivorController
*       Description		        : PlayerController for Ingame
*       LastUpate				: 2025/07/29
*       Todo                       :
*/

/*
class UInputMappingContext;
class UInputAction;
class UGameAudioSettingsWidget;
class ANetworkPlayer;
*/

UCLASS(Blueprintable)
class UNREALPHOBIA_API ASurvivorController : public APlayerController
{
	GENERATED_BODY()

public:
    /*
    ASurvivorController();
    
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ToggleAudioSettings(const FInputActionValue& Value);

    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowAudioSettings();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void HideAudioSettings();

    UFUNCTION(BlueprintPure, Category = "UI")
    bool IsInUIMode() const { return bIsUIMode; };

protected:
    virtual void BeginPlay() override;
    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnUnPossess() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputMappingContext> UIMappingContext;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputMappingContext> SurvivorMovingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UGameAudioSettingsWidget> AudioSettingsWidgetClass;

    UPROPERTY()
    UGameAudioSettingsWidget* AudioSettingsWidget;

private:
    bool bIsAudioSettingsVisible = false;
    bool bIsUIMode = false;

    void SetupEnhancedInput();
    void CreateAudioSettingsWidget();
    */
};