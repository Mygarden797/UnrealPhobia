#include "Settings/GameAudioSettings.h"
#include "Engine/Engine.h"

/**
*       Name				        : GameAudioSettings
*       Description		        : Manage general audio settings like volume
*       LastUpate				: 2025/08/06
*       Todo                       : Implement ApplyGameAudio()
*/

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

UGameAudioSettings* UGameAudioSettings::GetGameAudioSettings()
{
    return Cast<UGameAudioSettings>(UGameUserSettings::GetGameUserSettings());
}


// 전체 볼륨 재설정, 각 AudioCategory의 볼륨은 Master 볼륨에 종속적임
void UGameAudioSettings::SetMasterVolume(float Volume)
{
    MasterVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
    SetMusicVolume(MusicVolume);
    SetSFXVolume(SFXVolume);
    SetUIVolume(UIVolume);
}

void UGameAudioSettings::SetMusicVolume(float Volume)
{
    MusicVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
    ApplySoundMixOverride(AudioAssets->MusicSoundClass, MusicVolume);
}

void UGameAudioSettings::SetSFXVolume(float Volume)
{
    SFXVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
    ApplySoundMixOverride(AudioAssets->SFXSoundClass, SFXVolume);
}

void UGameAudioSettings::SetUIVolume(float Volume)
{
    UIVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
    ApplySoundMixOverride(AudioAssets->UISoundClass, UIVolume);
}

    void UGameAudioSettings::ApplyAudioSettings()
    {
        SetMasterVolume(MasterVolume);
        SaveSettings();
    }

    void UGameAudioSettings::ResetAudioToDefaults()
    {
        MasterVolume = 0.5f;
        MusicVolume = 0.5f;
        SFXVolume = 0.5f;
        UIVolume = 0.5f;

        SetMasterVolume(MasterVolume);
        SaveSettings();
    }

    bool UGameAudioSettings::IsAudioAssetsReady(USoundClass* SoundClass) const
    {
        return AudioAssets && AudioAssets->GlobalSoundMix && SoundClass;
    }

    void UGameAudioSettings::ApplySoundMixOverride(USoundClass* SoundClass, float Volume) 
    {
        UWorld* World = GetWorld();
        if (!IsAudioAssetsReady(SoundClass))
        {
            UE_LOG(LogTemp, Error, TEXT("UGameAudioSettings::ApplySoundMixOverride: Some Assets is null"));
            return;
        }
        if (!World)
        {
            UE_LOG(LogTemp, Error, TEXT("UGameAudioSettings::ApplySoundMixOverride: Failed to get World"));
            return;
        }

        UGameplayStatics::SetSoundMixClassOverride(
            World, AudioAssets->GlobalSoundMix, SoundClass, MasterVolume * Volume);
    }