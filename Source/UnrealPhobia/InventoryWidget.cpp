// Fill out your copyright notice in the Description page of Project Settings.

// InventoryWidget.cpp
#include "InventoryWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"

void UInventoryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    Super::NativeConstruct();

    // 1) 컴포넌트 찾아오기
    if (APlayerController *PC = GetOwningPlayer())
    {
        if (APawn *Pawn = PC->GetPawn())
        {
            InvComp = Pawn->FindComponentByClass<UTriggerInventory>();
        }
    }

    if (!InvComp)
        return;

    RefreshInventory();
}

void UInventoryWidget::RefreshInventory()
{
    if (!InvComp || !SlotGrid || !*InventorySlotClass)
        return;

    const TArray<ETriggerName> &Slots = InvComp->Inventory;

    SlotGrid->ClearChildren();
    const int32 Cols = 4;
    for (int32 Index = 0; Index < Slots.Num(); ++Index)
    {
        UInventorySlotWidget *SlotW = CreateWidget<UInventorySlotWidget>(GetWorld(), InventorySlotClass);
        SlotW->InitSlot(Slots[Index]);
        int32 Row = Index / Cols;
        int32 Col = Index % Cols;
        SlotGrid->AddChildToUniformGrid(SlotW, Row, Col);
    }
}