// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Trigger/Trigger.h"
#include "InventorySlotWidget.generated.h"

class UImage;
/**
 *
 */
UCLASS()
class UNREALPHOBIA_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void InitSlot(ETriggerName InTrigger, bool bIsSelected);

protected:
	// 디자이너에서 바인딩
	UPROPERTY(meta = (BindWidget))
	UImage *SlotImage;

	UPROPERTY(meta = (BindWidget))
	UImage *SlotBackgroundImage;
	// Enum → Texture 매핑 테이블
	UPROPERTY(EditAnywhere, Category = "Trigger")
	TMap<ETriggerName, TSoftObjectPtr<UTexture2D>> TriggerIcons;
	UPROPERTY(EditAnywhere, Category = "Background")
	TMap<bool, TSoftObjectPtr<UTexture2D>> BackgroundImages;
};