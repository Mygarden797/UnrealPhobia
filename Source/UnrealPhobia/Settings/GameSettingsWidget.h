// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "GameSettingsWidget.generated.h"

/**
*           Name				: GameSettingsWidget
*           Description		: Widget of Super Class with Environment Settings
*           LastUpdate	    : 2025/08/06
*/
UCLASS()
class UNREALPHOBIA_API UGameSettingsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

protected:

    /* Buttons */
    UPROPERTY(meta = (BindWidget))
    UButton* ApplyButton;

    UPROPERTY(meta = (BindWidget))
    UButton* ResetButton;

    UPROPERTY(meta = (BindWidget))
    UButton* BackButton;

    /* Click Functions */
    UFUNCTION()
    virtual void OnApplyClicked();

     UFUNCTION()
    virtual void OnResetClicked();

    UFUNCTION()
    virtual void OnBackClicked();

    /** Derived Classes' Fucntions 
    * 현재는 GameAudioSettingsWidget에서만 사용 중
    */
    virtual void LoadCurrentSettings() {};
    virtual void ApplyCurrentSettings() {};
    virtual void ResetCurrentSettings() {};
};
