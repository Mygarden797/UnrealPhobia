// Fill out your copyright notice in the Description page of Project Settings.

#include "AttackCameraWidget.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/TextureRenderTarget2D.h"
#include "TimerManager.h"

void UAttackCameraWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // ÃÊ±â¿¡´Â À§Á¬À» ¼û±è
    SetVisibility(ESlateVisibility::Hidden);

    // µ¿Àû ¸ÓÅÍ¸®¾ó »ý¼º
    if (RenderTargetMaterial && AttackCameraImage)
    {
        DynamicMaterial = UMaterialInstanceDynamic::Create(RenderTargetMaterial, this);
        if (DynamicMaterial)
        {
            AttackCameraImage->SetBrushFromMaterial(DynamicMaterial);
        }
    }
}

void UAttackCameraWidget::ShowAttackCamera(UTextureRenderTarget2D* RenderTarget, float ShowTime)
{
    if (!RenderTarget || !AttackCameraImage || !DynamicMaterial)
    {
        UE_LOG(LogTemp, Warning, TEXT("AttackCameraWidget: Missing required components"));
        return;
    }

    // À§Á¬ Ç¥½Ã
    SetVisibility(ESlateVisibility::Visible);

    // ·»´õ Å¸°Ù Á¤º¸ »ó¼¼ È®ÀÎ
    //UE_LOG(LogTemp, Warning, TEXT("=== RenderTarget Analysis ==="));
    //UE_LOG(LogTemp, Warning, TEXT("Size: %dx%d"), RenderTarget->SizeX, RenderTarget->SizeY);
    //UE_LOG(LogTemp, Warning, TEXT("Format: %d"), (int32)RenderTarget->RenderTargetFormat);
    //UE_LOG(LogTemp, Warning, TEXT("Resource Valid: %s"), RenderTarget->GetResource() ? TEXT("TRUE") : TEXT("FALSE"));

    // ·»´õ Å¸°ÙÀ» ¸ÓÅÍ¸®¾ó¿¡ ¼³Á¤
    DynamicMaterial->SetTextureParameterValue(TEXT("RenderTarget"), RenderTarget);


    // ÀÚµ¿ ¼û±è Å¸ÀÌ¸Ó ¼³Á¤
    if (ShowTime > 0.0f)
    {
        GetWorld()->GetTimerManager().SetTimer(
            HideTimerHandle,
            this,
            &UAttackCameraWidget::OnHideTimerEnd,
            ShowTime,
            false
        );
    }

    UE_LOG(LogTemp, Log, TEXT("Attack Camera Widget Shown"));
}

void UAttackCameraWidget::HideAttackCamera()
{
    // À§Á¬ ¼û±è
    SetVisibility(ESlateVisibility::Hidden);

    // Å¸ÀÌ¸Ó Å¬¸®¾î
    if (GetWorld()->GetTimerManager().IsTimerActive(HideTimerHandle))
    {
        GetWorld()->GetTimerManager().ClearTimer(HideTimerHandle);
    }

    UE_LOG(LogTemp, Log, TEXT("Attack Camera Widget Hidden"));
}

void UAttackCameraWidget::OnHideTimerEnd()
{
    HideAttackCamera();
}