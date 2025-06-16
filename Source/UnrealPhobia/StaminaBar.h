// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StaminaBar.generated.h"

class UProgressBar;

/**
 *
 */
UCLASS()
class UNREALPHOBIA_API UStaminaBar : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetStaminaPercent(float Percent);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UProgressBar *StaminaBar;
};
