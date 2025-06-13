// Fill out your copyright notice in the Description page of Project Settings.
// InventorSlotWidget.cpp
#include "InventorySlotWidget.h"
#include "Components/Image.h"

void UInventorySlotWidget::InitSlot(ETriggerName InTrigger, bool bIsSelected)
{
    if (!SlotImage)
    {
        UE_LOG(LogTemp, Warning, TEXT("SlotImage Binding Failed!"));
        return;
    }
    if (!SlotBackgroundImage)
    {
        UE_LOG(LogTemp, Warning, TEXT("SlotBackgroundImage Binding Failed!"));
        return;
    }
    else
        // UE_LOG(LogTemp, Log, TEXT("SlotBackgroundImage is Successfully Bound"));

        if (TriggerIcons.Contains(InTrigger))
        {
            // 동기 로드
            UTexture2D *Icon = TriggerIcons[InTrigger].LoadSynchronous();
            // UImage 헬퍼로 바로 세팅 (두 번째 인자는 bMatchSize)
            SlotImage->SetBrushFromTexture(Icon, true);

            // UE_LOG(LogTemp, Log, TEXT("SlotImage is Successfully Bound"));
        }
        else
        { // 빈 슬롯은 기본 브러시로 리셋
            SlotImage->SetBrush(FSlateNoResource());
        }

    if (BackgroundImages.Contains(bIsSelected))
    {
        // UE_LOG(LogTemp, Log, TEXT("BackgroundImage is Successfully Bound"));
        UTexture2D *Background = BackgroundImages[bIsSelected].LoadSynchronous();
        SlotBackgroundImage->SetBrushFromTexture(Background, true);
    }
    else
    {
        SlotBackgroundImage->SetBrush(FSlateNoResource());
    }
}
