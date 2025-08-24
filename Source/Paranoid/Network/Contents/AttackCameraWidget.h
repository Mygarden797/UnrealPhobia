// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "AttackCameraWidget.generated.h"

/**
 * 공격 카메라 화면을 표시하는 위젯
 */
UCLASS()
class PARANOID_API UAttackCameraWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // 위젯 초기화
    virtual void NativeConstruct() override;

    // 공격 카메라 화면 표시
    UFUNCTION(BlueprintCallable, Category = "Attack Camera")
    void ShowAttackCamera(UTextureRenderTarget2D* RenderTarget, float ShowTime = 3.0f);

    // 공격 카메라 화면 숨기기
    UFUNCTION(BlueprintCallable, Category = "Attack Camera")
    void HideAttackCamera();

protected:
    // 카메라 화면을 표시할 이미지 위젯
    UPROPERTY(meta = (BindWidget))
    class UImage* AttackCameraImage;

    // 렌더 타겟을 표시하기 위한 머터리얼
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Camera")
    class UMaterialInterface* RenderTargetMaterial;

    // 동적 머터리얼 인스턴스
    UPROPERTY()
    class UMaterialInstanceDynamic* DynamicMaterial;

    // 표시 시간을 위한 타이머
    FTimerHandle HideTimerHandle;

    // 타이머 콜백 함수
    void OnHideTimerEnd();
};