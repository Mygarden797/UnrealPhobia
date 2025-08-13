#include "Settings/GameSettingsWidget.h"

void UGameSettingsWidget::NativeConstruct()
{
    Super::NativeConstruct();
    bIsFocusable = true;

    // 각 버튼에 대한 바인딩
    if (ApplyButton)
    {
        ApplyButton->OnClicked.AddDynamic(this, &UGameSettingsWidget::OnApplyClicked);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UGameSettingsWidget::NativeContruct(): Failed to bind ApplyButton"));
    }

    if (ResetButton)
    {
        ResetButton->OnClicked.AddDynamic(this, &UGameSettingsWidget::OnResetClicked);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UGameSettingsWidget::NativeContruct(): Failed to bind ResetButton"));
    }

    if (BackButton)
    {
        BackButton->OnClicked.AddDynamic(this, &UGameSettingsWidget::OnBackClicked);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UGameSettingsWidget::NativeContruct(): Failed to bind BackButton"));
    }

    LoadCurrentSettings();
    
}

void UGameSettingsWidget::OnApplyClicked()
{
    ApplyCurrentSettings();
    UE_LOG(LogTemp, Warning, TEXT("UGameSettingsWidget::OnApplyClicked(): Settings Applied"));
}

void UGameSettingsWidget::OnResetClicked()
{
    ResetCurrentSettings();
    LoadCurrentSettings();
    UE_LOG(LogTemp, Warning, TEXT("UGameSettingsWidget::OnResetClicked(): Settings Reset"));
}

void UGameSettingsWidget::OnBackClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("UGameSettingsWidget::OnBackClicked(): Go back"));
    // RemoveFromParent();
    UE_LOG(LogTemp, Warning, TEXT("=== 1. Back Button Clicked in Widget! ==="));
    OnBackRequested.ExecuteIfBound();
}