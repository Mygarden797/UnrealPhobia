#include "Settings/GameAudioSettings.h"
#include "Managers/SoundManager.h"

#include "Engine/Engine.h"


/**
*       Name				        : GameAudioSettings
*       Description		        : Manage general audio settings like volume
*       LastUpate				: 2025/08/05
*       Todo                       : Implement ApplyGameAudio()
*/

UGameAudioSettings::UGameAudioSettings()
{
    MasterVolume = 0.5f;
    MusicVolume = 0.5f;
    SFXVolume = 0.5f;
    UIVolume = 0.5f;
}

UGameAudioSettings* UGameAudioSettings::GetGameAudioSettings()
{
    return Cast<UGameAudioSettings>(UGameUserSettings::GetGameUserSettings());
}

void UGameAudioSettings::SetCategoryVolume(EAudioCategory Category, float Volume)
{
    switch (Category)
    {
    case EAudioCategory::Master:
        SetMasterVolume(Volume);
        break;
    case EAudioCategory::Music:
        SetMusicVolume(Volume);
        break;
    case EAudioCategory::SFX:
        SetSFXVolume(Volume);
        break;
    case EAudioCategory::UI:
        SetUIVolume(Volume);
        break;
    default:
        UE_LOG(LogTemp, Error, TEXT("UGameAudioSettings::SetCategoryVolume() : Invaild Category Type"));
        break;
    }
}

void UGameAudioSettings::SetMasterVolume(float Volume)
{
    ClampVolume(Volume);
    if (MasterVolume != Volume)
    {
        MasterVolume = Volume;
        // ApplyAudioSettings();
    }
}

void UGameAudioSettings::SetMusicVolume(float Volume)
{
    ClampVolume(Volume);
    if (MusicVolume != Volume)
    {
        MusicVolume = Volume;
        // ApplyAudioSettings();
    }
}

void UGameAudioSettings::SetSFXVolume(float Volume)
{
    ClampVolume(Volume);
    if (SFXVolume != Volume)
    {
        SFXVolume = Volume;
        // ApplyAudioSettings();
    }
}

void UGameAudioSettings::SetUIVolume(float Volume)
{
    ClampVolume(Volume);
    if (UIVolume != Volume)
    {
        UIVolume = Volume;
        // ApplyAudioSettings();
    }
}

float UGameAudioSettings::GetCategoryVolume(EAudioCategory Category) const
{
    switch (Category)
    {
    case EAudioCategory::Master:
        return GetMasterVolume();
    case EAudioCategory::Music:
        return GetMusicVolume();
    case EAudioCategory::SFX:
        return GetSFXVolume();
    case EAudioCategory::UI:
        return GetUIVolume();
    default:
        UE_LOG(LogTemp, Error, TEXT("UGameAudioSettings::GetCategoryVolume() : Invaild Category Type"));
        return 1.0f;
    }
}

void UGameAudioSettings::ApplySettings(bool bCheckForCommandLineOverrides)
{
    Super::ApplySettings(bCheckForCommandLineOverrides);
    // ApplyAudioSettings();
}

void UGameAudioSettings::SetToDefaults()
{
    Super::SetToDefaults();

    MasterVolume = 0.5f;
    MusicVolume = 0.5f;
    SFXVolume = 0.5f;
    UIVolume = 0.5f;

    // ApplyAudioSettings();
}

// 변경된 오디오 세팅을 사운드 매니저에 전달한다
void UGameAudioSettings::ApplyAudioSettings()
{
    if (UWorld* World = GEngine->GetCurrentPlayWorld())
    {
        if (UGameInstance* GameInstance = World->GetGameInstance())
        {
            if (USoundManager* SoundManager = GameInstance->GetSubsystem<USoundManager>())
            {
                SoundManager->ApplyAudioSettings();
            }
        }
    }
}

void UGameAudioSettings::ClampVolume(float& Volume)
{
    Volume = FMath::Clamp(Volume, 0.0f, 1.0f);
}