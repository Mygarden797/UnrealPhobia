#include "Settings/GameAudioSettingsWidget.h"
#include "GameAudioSettings.h"
#include "GameFramework/GameUserSettings.h"


// 위젯이 뷰포트에 추가된 후 호출되는 생성자
void UGameAudioSettingsWidget::NativeConstruct()
{
    Super::NativeConstruct();
    BindSliderEvents();
}

void UGameAudioSettingsWidget::BindSliderEvents()
{
    struct SliderBinding
    {
        USlider** Slider;
        void (UGameAudioSettingsWidget::* Function)(float);
        const TCHAR* Name;
    };

    // Struct Array로 저장
    SliderBinding Bindings[] = {
        {&MasterSlider, &UGameAudioSettingsWidget::OnMasterVolumeChanged,TEXT("MasterSlide")},
        {&MusicSlider, &UGameAudioSettingsWidget::OnMusicVolumeChanged,TEXT("MusicSlide")},
        {&SFXSlider, &UGameAudioSettingsWidget::OnSFXVolumeChanged,TEXT("SFXSlide")},
        {&UISlider, &UGameAudioSettingsWidget::OnUIVolumeChanged,TEXT("UISlide")}
    };

    // 각 AudioCategory에 대한 OnVolumeChanged()를 각각 바인딩
    for (const auto& Binding : Bindings)
    {
        if (*Binding.Slider)
        {
            (*Binding.Slider)->OnValueChanged.AddDynamic(this, Binding.Function);
        }
        else
        {
            UE_LOG(LogTemp, Error, 
                TEXT("UGameAudioSettingsWidget::BindSliderEvents(): No %s"), 
                Binding.Name);
        }
    }
}

// AudioSettings에서 볼륨 변경 및 슬라이더의 값 변경
void UGameAudioSettingsWidget::OnVolumeChanged(float Value, EAudioCategory Type)
{
    UGameAudioSettings* Settings = GetAudioSettings();
    if (!Settings)
    {
        UE_LOG(LogTemp, Error,
            TEXT("UGameAudioSettingsWidget::OnVolumeChanged(): Settings is null"));
        return;
    }
    SetVolumeByType(Settings, Type, Value);
    UpdatePercentageText(GetTextBlockByType(Type), Value);
}

UGameAudioSettings* UGameAudioSettingsWidget::GetAudioSettings() const
{
    UGameAudioSettings* Settings =
        Cast<UGameAudioSettings>(UGameUserSettings::GetGameUserSettings());

    if (!Settings)
    {
        UE_LOG(LogTemp, Error,
            TEXT("UGameAudioSettingsWidget::GetAudioSettings(): Failed to get AudioSettings"));
    }
    return Settings;
}

// AudioSettings에서 각 AudioCategory의 볼륨 변경
void UGameAudioSettingsWidget::SetVolumeByType(
    UGameAudioSettings* Settings, EAudioCategory Type, float Value)
{
    switch (Type)
    {
    case EAudioCategory::Master:
        Settings->SetMasterVolume(Value);
        break;
    case EAudioCategory::Music:
        Settings->SetMusicVolume(Value);
        break;
    case EAudioCategory::SFX:
        Settings->SetSFXVolume(Value);
        break;
    case EAudioCategory::UI:
        Settings->SetUIVolume(Value);
        break;
    default:
        UE_LOG(LogTemp, Error,
            TEXT("UGameAudioSettingsWidget::SetVolumeByType(): Wrong Audio Type"));
    }
}

float UGameAudioSettingsWidget::GetVolumeByType(
    UGameAudioSettings* Settings, EAudioCategory Type) const
{
    switch (Type)
    {
    case EAudioCategory::Master:
        return Settings->GetMasterVolume();
    case EAudioCategory::Music:
        return Settings->GetMusicVolume();
    case EAudioCategory::SFX:
        return Settings->GetSFXVolume();
    case EAudioCategory::UI:
        return Settings->GetUIVolume();
    default:
        UE_LOG(LogTemp, Error,
            TEXT("UGameAudioSettingsWidget::GetVolumeByType(): Wrong Audio Type"));
        return 0.0f;
    }
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
    UGameAudioSettings* Settings = GetAudioSettings();
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
    if (UGameAudioSettings* Settings = GetAudioSettings())
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
    if (UGameAudioSettings* Settings = GetAudioSettings())
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