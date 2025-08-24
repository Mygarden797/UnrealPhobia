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

    if (TriggerIcons.Contains(InTrigger))
    {
        UTexture2D *Icon = TriggerIcons[InTrigger].LoadSynchronous();
        if (Icon)
        {
            FSlateBrush Brush;
            Brush.SetResourceObject(Icon);
            Brush.ImageSize = FVector2D(240, 240);

            SlotImage->SetBrush(Brush);
            SlotImage->SetDesiredSizeOverride(Brush.ImageSize); // 추가 안전장치
        }
        else
        {
            SlotImage->SetBrush(FSlateNoResource());
        }
    }
    else
    {
        SlotImage->SetBrush(FSlateNoResource());
    }

    if (BackgroundImages.Contains(bIsSelected))
    {
        UTexture2D *Background = BackgroundImages[bIsSelected].LoadSynchronous();
        if (Background)
        {
            FSlateBrush BgBrush;
            BgBrush.SetResourceObject(Background);
            BgBrush.ImageSize = FVector2D(240, 240);
            SlotBackgroundImage->SetBrush(BgBrush);
        }
        else
        {
            SlotBackgroundImage->SetBrush(FSlateNoResource());
        }
    }
    else
    {
        SlotBackgroundImage->SetBrush(FSlateNoResource());
    }
}
