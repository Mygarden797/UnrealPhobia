#include "Settings/GameAudioSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

UGameAudioSettings::UGameAudioSettings()
{
    MasterVolume = 0.5f;
    MusicVolume = 0.5f;
    SFXVolume = 0.5f;
    UIVolume = 0.5f;
}

UGameAudioSettings* UGameAudioSettings::GetGameAudioSettings()
{
    if (UGameUserSettings* Settings = GEngine->GetGameUserSettings())
    {
        // UE_LOG(LogTemp, Warning, TEXT("Class path: %s"), *Settings->GetClass()->GetPathName());
        // UE_LOG(LogTemp, Warning, TEXT("IsA<UGameAudioSettings>: %d"), Settings->IsA(UGameAudioSettings::StaticClass()));

        // UE_LOG(LogTemp, Warning, TEXT("Loaded class path: %s"), *Settings->GetClass()->GetPathName());
        // UE_LOG(LogTemp, Warning, TEXT("Loaded class outer: %s"), *Settings->GetClass()->GetOuter()->GetName());
    }

    // UE_LOG(LogTemp, Log, TEXT("%s"), *UGameAudioSettings::StaticClass()->GetPathName());
    if (GEngine)
        return Cast<UGameAudioSettings>(GEngine->GetGameUserSettings());
    return nullptr;
}


// 전체 볼륨 재설정, 각 AudioCategory의 볼륨은 Master 볼륨에 종속적임
void UGameAudioSettings::SetVolume(EAudioCategory Category, float Value)
{
    float ClampedValue = FMath::Clamp(Value, 0.f, 1.f);
    switch (Category)
    {
    case EAudioCategory::Master:
        MasterVolume = ClampedValue;
        break;
    case EAudioCategory::Music:
        MusicVolume = ClampedValue;
        break;
    case EAudioCategory::SFX:
        SFXVolume = ClampedValue;
        break;
    case EAudioCategory::UI:
        UIVolume = ClampedValue;
        break;
    default:
        UE_LOG(LogTemp, Error, TEXT("UGameAudioSettings::GetVolume(): Wrong AudioCategory Type"));
        break;
    }

    OnAudioSettingsChanged.Broadcast();
    // UE_LOG(LogTemp, Warning, TEXT("=== 2. Settings Broadcasted! ==="));
}

float UGameAudioSettings::GetVolume(EAudioCategory Category) const
{
    switch (Category)
    {
    case EAudioCategory::Master:
        return MasterVolume;
    case EAudioCategory::Music:
        return MusicVolume;
    case EAudioCategory::SFX:
        return SFXVolume;
    case EAudioCategory::UI:
        return UIVolume;
    default:
        UE_LOG(LogTemp, Error, TEXT("UGameAudioSettings::GetVolume(): Wrong AudioCategory Type"));
        return 1.f;
    }
}

void UGameAudioSettings::ResetAudioToDefaultsOnly()
{
    MasterVolume = 0.5f;
    MusicVolume = 0.5f;
    SFXVolume = 0.5f;
    UIVolume = 0.5f;

    OnAudioSettingsChanged.Broadcast();
    SaveSettings();
}

/* Will be used when make whole of settings
void UGameAudioSettings::SetToDefaults()
{
    Super::SetToDefaults();

    MasterVolume = 0.5f;
    MusicVolume = 0.5f;
    SFXVolume = 0.5f;
    UIVolume = 0.5f;
}
*/