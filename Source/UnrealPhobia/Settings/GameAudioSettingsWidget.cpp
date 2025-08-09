#include "Settings/GameAudioSettingsWidget.h"
#include "GameAudioSettings.h"
#include "GameFramework/GameUserSettings.h"


// 위젯이 뷰포트에 추가된 후 호출되는 생성자
void UGameAudioSettingsWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (UGameAudioSettings* Settings = UGameAudioSettings::GetGameAudioSettings())
    {
        BindSliderEvents();
    }
}

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

    /*
struct SliderBinding
{
    USlider* Slider;
    FName FunctionName;
    const TCHAR* Name;
};

// Struct Array로 저장
SliderBinding Bindings[] = {
    {MasterSlider, TEXT("OnMasterVolumeChanged"), TEXT("MasterSlider")},
    {MusicSlider, TEXT("OnMusicVolumeChanged"), TEXT("MusicSlider")},
    {SFXSlider, TEXT("OnSFXVolumeChanged"), TEXT("SFXSlider")},
    {UISlider, TEXT("OnUIVolumeChanged"), TEXT("UISlider")},


    {MasterSlider, &UGameAudioSettingsWidget::OnMasterVolumeChanged,TEXT("MasterSlide")},
    {MusicSlider, &UGameAudioSettingsWidget::OnMusicVolumeChanged,TEXT("MusicSlide")},
    {SFXSlider, &UGameAudioSettingsWidget::OnSFXVolumeChanged,TEXT("SFXSlide")},
    {UISlider, &UGameAudioSettingsWidget::OnUIVolumeChanged,TEXT("UISlide")}

};

// 각 AudioCategory에 대한 OnVolumeChanged()를 각각 바인딩
for (const auto& Binding : Bindings)
{
    if (Binding.Slider)
    {
        (Binding.Slider)->OnValueChanged.AddDynamic(this, Binding.FunctionName);
    }
    else
    {
        UE_LOG(LogTemp, Error,
            TEXT("UGameAudioSettingsWidget::BindSliderEvents(): No %s"),
            Binding.Name);
    }
}*/
}

// AudioSettings에서 볼륨 변경 및 슬라이더의 값 변경
void UGameAudioSettingsWidget::OnVolumeChanged(float Value, EAudioCategory Type)
{
    UGameAudioSettings* Settings = UGameAudioSettings::GetGameAudioSettings();
    if (!Settings)
    {
        UE_LOG(LogTemp, Error,
            TEXT("UGameAudioSettingsWidget::OnVolumeChanged(): Settings is null"));
        return;
    }
    SetVolumeByType(Settings, Type, Value);
    UpdatePercentageText(GetTextBlockByType(Type), Value);
}


// AudioSettings에서 각 AudioCategory의 볼륨 변경
void UGameAudioSettingsWidget::SetVolumeByType(
    UGameAudioSettings* Settings, EAudioCategory Type, float Value)
{
    Settings->SetVolume(Type, Value);
}

float UGameAudioSettingsWidget::GetVolumeByType(
    UGameAudioSettings* Settings, EAudioCategory Type) const
{
    return Settings->GetVolume(Type);
}

USlider* UGameAudioSettingsWidget::GetSliderByType(EAudioCategory Type) const
{
    switch (Type)
    {
    case EAudioCategory::Master:
        return MasterSlider;
    case EAudioCategory::Music:
        return MusicSlider;
    case EAudioCategory::SFX:
        return SFXSlider;
    case EAudioCategory::UI:
        return UISlider;
    default:
        UE_LOG(LogTemp, Error,
            TEXT("UGameAudioSettingsWidget::GetSliderByType(): Wrong Audio Type"));
        return nullptr;
    }
}

UTextBlock* UGameAudioSettingsWidget::GetTextBlockByType(EAudioCategory Type) const
{
    switch (Type)
    {
    case EAudioCategory::Master:
        return MasterPercent;
    case EAudioCategory::Music:
        return MusicPercent;
    case EAudioCategory::SFX:
        return SFXPercent;
    case EAudioCategory::UI:
        return UIPercent;
    default:
        UE_LOG(LogTemp, Error,
            TEXT("UGameAudioSettingsWidget::GetTextBlockByType(): Wrong Audio Type"));
        return nullptr;
    }
}

void UGameAudioSettingsWidget::LoadCurrentSettings()
{
    UGameAudioSettings* Settings = UGameAudioSettings::GetGameAudioSettings();
    if (!Settings) return;

    EAudioCategory Types[] =
    { EAudioCategory::Master, EAudioCategory::Music,
    EAudioCategory::SFX, EAudioCategory::UI };

    for (EAudioCategory Type : Types)
    {
        USlider* Slider = GetSliderByType(Type);
        if (Slider)
        {
            float Volume = GetVolumeByType(Settings, Type);
            Slider->SetValue(Volume);
            UpdatePercentageText(GetTextBlockByType(Type), Volume);
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
        Settings->SetToDefaults();
        Settings->SaveSettings();
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
        FString PercentText = FString::Printf(TEXT("%.0f%%"), Value * 100.0f);
        TextWidget->SetText(FText::FromString(PercentText));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UGameAudioSettingsWidget::UpdatePercentageText(): No TextWidget"))
    }
}