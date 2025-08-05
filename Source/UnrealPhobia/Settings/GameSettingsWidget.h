// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "GameSettingsWidget.generated.h"

/**
 * 
 */
UCLASS()
class UNREALPHOBIA_API UGameSettingsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
 //   virtual void NativeConstruct() override;

protected:

    /* Buttons */
    UPROPERTY(meta = (BindWidget))
    UButton* ApplyButton;

    UPROPERTY(meta = (BindWidget))
    UButton* ResetButton;

    UPROPERTY(meta = (BindWidget))
    UButton* BackButton;

    /* Click Functions */

    /*
    UFUNCTION()
    virtual void OnApplyClicked();

     UFUNCTION()
    virtual void OnResetClicked();

    UFUNCTION()
    virtual void OnBackClicked();

    */

    /* Derived Classes' Fucntions */

    /*
    UFUNCTION(BlueprintImplementableEvent)
    virtual void LoadCurrentSettings() {}

    UFUNCTION(BlueprintImplementableEvent)
    virtual void ApplyCurrentSettings() {}

    UFUNCTION(BlueprintImplementableEvent)
    virtual void ResetCurrentSettings() {}
    */
	
};
