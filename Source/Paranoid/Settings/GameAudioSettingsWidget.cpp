#include "Settings/GameAudioSettingsWidget.h"
#include "GameAudioSettings.h"
#include "GameFramework/GameUserSettings.h"


// 위젯이 뷰포트에 추가된 후 호출되는 생성자
void UGameAudioSettingsWidget::NativeConstruct()
{
    Super::NativeConstruct();
    BindSliderEvents();
    LoadCurrentSettings();
    /*
    if (UGameAudioSettings* Settings = UGameAudioSettings::GetGameAudioSettings())
    {
        Settings->OnAudioSettingsChanged.AddDynamic(this, &UGameAudioSettingsWidget::OnAudioSettingsChanged);
    }
    */
}

// 슬라이더 바인딩
void UGameAudioSettingsWidget::BindSliderEvents()
{
    if (MasterSlider)
    {
        MasterSlider->OnValueChanged.AddDynamic(this, &UGameAudioSettingsWidget::OnMasterVolumeChanged);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UGameAudioSettingsWidget::BindSliderEvents(): Failed to bind MasterSlider"));
    }

    if (MusicSlider)
    {
        MusicSlider->OnValueChanged.AddDynamic(this, &UGameAudioSettingsWidget::OnMusicVolumeChanged);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UGameAudioSettingsWidget::BindSliderEvents(): Failed to bind MasterSlider"));
    }

    if (SFXSlider)
    {
        SFXSlider->OnValueChanged.AddDynamic(this, &UGameAudioSettingsWidget::OnSFXVolumeChanged);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UGameAudioSettingsWidget::BindSliderEvents(): Failed to bind MasterSlider"));
    }

    if (UISlider)
    {
        UISlider->OnValueChanged.AddDynamic(this, &UGameAudioSettingsWidget::OnUIVolumeChanged);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UGameAudioSettingsWidget::BindSliderEvents(): Failed to bind MasterSlider"));
    }

}

void UGameAudioSettingsWidget::LoadCurrentSettings()
{
    if (UGameAudioSettings* Settings = UGameAudioSettings::GetGameAudioSettings())
    {
        if (IsValid(Settings))
        {
            if (bIsDelegateBound)
            {
                Settings->OnAudioSettingsChanged.AddDynamic(this, 
                    &UGameAudioSettingsWidget::OnAudioSettingsChanged);
                bIsDelegateBound = true;
                // UE_LOG(LogTemp, Log, TEXT("GameAudioSettingsWidget: Successfully performed lazy delegate binding."));
            }
            const TMap<EAudioCategory, TPair<USlider*, UTextBlock*>> UIMap =
            {
                { EAudioCategory::Master, { MasterSlider, MasterPercent } },
                { EAudioCategory::Music, { MusicSlider, MusicPercent } },
                { EAudioCategory::SFX, { SFXSlider, SFXPercent } },
                { EAudioCategory::UI, { UISlider, UIPercent } },
            };

            for (const TPair<EAudioCategory, TPair<USlider*, UTextBlock*>>& Pair : UIMap)
            {
                const EAudioCategory Category = Pair.Key;
                USlider* Slider = Pair.Value.Key;
                UTextBlock* TextBlock = Pair.Value.Value;

                if (Slider && TextBlock)
                {
                    const float Volume = Settings->GetVolume(Category);
                    Slider->SetValue(Volume);
                    UpdatePercentageText(TextBlock, Volume);
                }
            }
        }
    }
}

void UGameAudioSettingsWidget::ApplyCurrentSettings()
{
    if (UGameAudioSettings* Settings = UGameAudioSettings::GetGameAudioSettings())
    {
        Settings->SaveSettings();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UGameAudioSettingsWidget::ApplyCurrentSettings(): Failed to cast"))
    }
}

void UGameAudioSettingsWidget::ResetCurrentSettings()
{
    if (UGameAudioSettings* Settings = UGameAudioSettings::GetGameAudioSettings())
    {
        Settings->ResetAudioToDefaultsOnly();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UGameAudioSettingsWidget::ResetCurrentSettings(): Failed to cast"))
    }
}

void UGameAudioSettingsWidget::UpdatePercentageText(UTextBlock* TextWidget, float Value)
{
    if (TextWidget)
    {
        int32 Percent = FMath::RoundToInt(Value * 100.0f);
        TextWidget->SetText(FText::FromString(FString::Printf(TEXT("%d%%"), Percent)));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UGameAudioSettingsWidget::UpdatePercentageText(): No TextWidget"))
    }
}

void UGameAudioSettingsWidget::OnAudioSettingsChanged()
{
    LoadCurrentSettings();
}

void UGameAudioSettingsWidget::OnMasterVolumeChanged(float Value)
{
    //UE_LOG(LogTemp, Warning, TEXT("=== 1. Widget Event Fired: Master Volume Changed to %.2f ==="), Value);
    if (UGameAudioSettings* Settings = UGameAudioSettings::GetGameAudioSettings())
    {
        Settings->SetVolume(EAudioCategory::Master, Value);
    }
    else
    {
        UE_LOG(LogTemp, Error,
            TEXT("UGameAudioSettingsWidget::OnVolumeChanged(): Settings is null"));
        return;
    }
    UpdatePercentageText(MasterPercent, Value);
}

void UGameAudioSettingsWidget::OnMusicVolumeChanged(float Value)
{  
    //UE_LOG(LogTemp, Warning, TEXT("=== 1. Widget Event Fired: Music Volume Changed to %.2f ==="), Value);
    if (UGameAudioSettings* Settings = UGameAudioSettings::GetGameAudioSettings())
    {
        Settings->SetVolume(EAudioCategory::Music, Value);
    }
    else
    {
        UE_LOG(LogTemp, Error,
            TEXT("UGameAudioSettingsWidget::OnVolumeChanged(): Settings is null"));
        return;
    }
    UpdatePercentageText(MusicPercent, Value);
}

void UGameAudioSettingsWidget::OnSFXVolumeChanged(float Value)
{
    // UE_LOG(LogTemp, Warning, TEXT("=== 1. Widget Event Fired: SFX Volume Changed to %.2f ==="), Value);
    if (UGameAudioSettings* Settings = UGameAudioSettings::GetGameAudioSettings())
    {
        Settings->SetVolume(EAudioCategory::SFX, Value);
    }
    else
    {
        UE_LOG(LogTemp, Error,
            TEXT("UGameAudioSettingsWidget::OnVolumeChanged(): Settings is null"));
        return;
    }
    UpdatePercentageText(SFXPercent, Value);
}

void UGameAudioSettingsWidget::OnUIVolumeChanged(float Value)
{
    // UE_LOG(LogTemp, Warning, TEXT("=== 1. Widget Event Fired: UI Volume Changed to %.2f ==="), Value);
    if (UGameAudioSettings* Settings = UGameAudioSettings::GetGameAudioSettings())
    {
        Settings->SetVolume(EAudioCategory::UI, Value);
    }
    else
    {
        UE_LOG(LogTemp, Error,
            TEXT("UGameAudioSettingsWidget::OnVolumeChanged(): Settings is null"));
        return;
    }
    UpdatePercentageText(UIPercent, Value);
}