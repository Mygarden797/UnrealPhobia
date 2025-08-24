// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Trigger/TriggerInventory.h"
#include "InventorySlotWidget.h"
#include "InventoryWidget.generated.h"

class UUniformGridPanel;

// InventoryWidget.h
UCLASS()
class PARANOID_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel *SlotGrid;

	// 슬롯 위젯 클래스
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UInventorySlotWidget> InventorySlotClass;

	// 플레이어의 컴포넌트 참조
	UPROPERTY()
	UTriggerInventory *InvComp;

public:
	void RefreshInventory();
};
