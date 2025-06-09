// Fill out your copyright notice in the Description page of Project Settings.
// InventorSlotWidget.cpp
#include "InventorySlotWidget.h"
#include "Components/Image.h"

void UInventorySlotWidget::InitSlot(ETriggerName InTrigger)
{
    if (!SlotImage)
    {
        UE_LOG(LogTemp, Warning, TEXT("SlotImage가 바인딩되지 않았습니다!"));
        return;
    }

    if (TriggerIcons.Contains(InTrigger))
    {
        // 동기 로드
        UTexture2D *Icon = TriggerIcons[InTrigger].LoadSynchronous();
        // UImage 헬퍼로 바로 세팅 (두 번째 인자는 bMatchSize)
        SlotImage->SetBrushFromTexture(Icon, true);
        UE_LOG(LogTemp, Warning, TEXT("SlotImage 바인딩 완료"));
    }
    else
    {
        // 빈 슬롯은 기본 브러시로 리셋
        SlotImage->SetBrush(FSlateNoResource());
    }
}
