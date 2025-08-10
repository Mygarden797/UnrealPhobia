#include "Settings/GameAudioSettings.h"
#include "Engine/Engine.h"
#include "Engine/GameEngine.h"


UGameAudioSettings::UGameAudioSettings()
{
    MasterVolume = 0.5f;
    MusicVolume = 0.5f;
    SFXVolume = 0.5f;
    UIVolume = 0.5f;

    AudioAssets = UAudioAssets::LoadAudioAssets();
    if (!AudioAssets)
    {
        UE_LOG(LogTemp, Error, TEXT("UGameAudioSettings::Constructor: Failed to load AudioAssets"));
    }

}

/*
UGameAudioSettings* UGameAudioSettings::CreateGameAudioSettings()
{
    if (GEngine)
    {
        if (UGameAudioSettings* ExistingSettings = Cast<UGameAudioSettings>(GEngine->GetGameUserSettings()))
        {
            return ExistingSettings;
        }
        else
        {
            UGameAudioSettings* NewSettings = NewObject<UGameAudioSettings>(GEngine);
            GEngine->GameUserSettings = NewSettings;
            return NewSettings;
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UGameAudioSettings::CreateGameAudioSettings(): GEngine is null"));
        return;
    }
}
*/

bool UGameAudioSettings::IsAudioAssetsReady(USoundClass* SoundClass) const
{
    return AudioAssets && AudioAssets->GlobalSoundMix && SoundClass;
}

UGameAudioSettings* UGameAudioSettings::GetGameAudioSettings()
{
    if (UGameUserSettings* Settings = GEngine->GetGameUserSettings())
    {
        UE_LOG(LogTemp, Warning, TEXT("Class path: %s"), *Settings->GetClass()->GetPathName());
        UE_LOG(LogTemp, Warning, TEXT("IsA<UGameAudioSettings>: %d"), Settings->IsA(UGameAudioSettings::StaticClass()));

        UE_LOG(LogTemp, Warning, TEXT("Loaded class path: %s"), *Settings->GetClass()->GetPathName());
        UE_LOG(LogTemp, Warning, TEXT("Loaded class outer: %s"), *Settings->GetClass()->GetOuter()->GetName());
    }

    UE_LOG(LogTemp, Log, TEXT("%s"), *UGameAudioSettings::StaticClass()->GetPathName());
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

    ApplyAllVolumes();
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

void UGameAudioSettings::ApplyAllVolumes()
{
    if (!AudioAssets)
    {
        UE_LOG(LogTemp, Error, TEXT("UGameAudioSettings::ApplyAllVolumes(): AudioAssets is null"));
    }

    ApplySoundMixOverride(AudioAssets->MusicSoundClass, MasterVolume * MusicVolume);
    ApplySoundMixOverride(AudioAssets->SFXSoundClass, MasterVolume * SFXVolume);
    ApplySoundMixOverride(AudioAssets->UISoundClass, MasterVolume * UIVolume);
}

void UGameAudioSettings::ResetAudioToDefaults()
{
    MasterVolume = 0.5f;
    MusicVolume = 0.5f;
    SFXVolume = 0.5f;
    UIVolume = 0.5f;

    ApplyAllVolumes();
    SaveSettings();
}

void UGameAudioSettings::ApplySoundMixOverride(USoundClass* SoundClass, float Volume)
{
    if (!IsAudioAssetsReady(SoundClass))
    {
        UE_LOG(LogTemp, Error, TEXT("UGameAudioSettings::ApplySoundMixOverride: Some Assets is null"));
        return;
    }
    UWorld* World = nullptr;
    if (GEngine && GEngine->GetWorldContexts().Num() > 0)
    {
        World = GEngine->GetWorldContexts()[0].World();
    }
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("UGameAudioSettings::ApplySoundMixOverride: Failed to get World"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("Applying volume %.2f to SoundClass %s"),
        Volume, *SoundClass->GetName());
    UGameplayStatics::SetSoundMixClassOverride(
        World, AudioAssets->GlobalSoundMix, SoundClass, Volume);
}
