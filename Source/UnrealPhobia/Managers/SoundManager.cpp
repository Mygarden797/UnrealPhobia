// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/SoundManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"
#include "Misc/ConfigCacheIni.h"


// Initalize External Resources
void USoundManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    // UE_LOG(LogTemp, Warning, TEXT("USoundManager::Initalize(), should be called only one time"));

    AudioAssets = UAudioAssets::LoadAudioAssets();
    AudioSettings = UGameAudioSettings::GetGameAudioSettings();

    if (IsValid(AudioSettings))
    {
        AudioSettings->OnAudioSettingsChanged.AddDynamic(this, &USoundManager::ApplyVolume);
        bIsDelegateBound = true;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("USoundManager::Initalize(): Failed to get AudioSettings"));
    }
    
    if (IsValidAssets() && AudioAssets->GlobalSoundMix)
    {
        UGameplayStatics::PushSoundMixModifier(this, AudioAssets->GlobalSoundMix);
        UE_LOG(LogTemp, Warning, TEXT("=== Global Sound Mix Activated! ==="));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("USoundManager::Initalize(): Failed to get AudioAssets or GlobalSoundMix"));
    }

    ApplyVolume();


    BeingChasedSource = UGameplayStatics::SpawnSound2D(GetWorld(), AudioAssets->BeingChased);
    if (BeingChasedSource && AudioAssets && AudioAssets->BeingChased)
    {
        BeingChasedSource->bAutoDestroy = false;
        BeingChasedSource->SetVolumeMultiplier(1.0f);
        UE_LOG(LogTemp, Display, TEXT("USoundManager::Initalize(): Set BeingChasedSource"));
    }
}

void USoundManager::Deinitialize()
{
    if (AudioSettings)
    {
        AudioSettings->OnAudioSettingsChanged.RemoveDynamic(this, &USoundManager::ApplyVolume);
    }
    Super::Deinitialize();
}


bool USoundManager::IsValidAssets() const
{
    if (!AudioAssets)
    {
        UE_LOG(LogTemp, Error, TEXT("USoundManager: AudioAssets is NULL!!"));
        return false;
    }
    return true;
}

// 플레이어에게 직접 전달
void USoundManager::PlayDetectedSound()
{
    if (IsValidAssets())
    {
        if (AudioAssets->BeDetected)
        {
            UGameplayStatics::PlaySound2D(GetWorld(), AudioAssets->BeDetected);
            UE_LOG(LogTemp, Display, TEXT("USoundManager::PlayDetectedSound()"));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("USoundManager::PlayDetectedSound(): No BeDetected"));
            return;
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("USoundManager::PlayDetectedSound(): No Assets"));
        return;
    }
}

void USoundManager::PlayScream()
{
    if (IsValidAssets())
    {
        if (AudioAssets->BeDetected)
        {
            UGameplayStatics::PlaySound2D(GetWorld(), AudioAssets->BeDetected);
            UE_LOG(LogTemp, Display, TEXT("USoundManager::PlayDetectedSound()"));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("USoundManager::PlayDetectedSound(): No BeDetected"));
            return;
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("USoundManager::PlayDetectedSound(): No Assets"));
        return;
    }
}

// 공간계에서 출력
void USoundManager::PlaySFX3D(UObject* Object, USoundBase* SFX, FVector Location, FRotator Rotation, USoundAttenuation* AttenuationSettings)
{
    if (!SFX) return;
    else
    {
        UGameplayStatics::PlaySoundAtLocation(
            Object,
            SFX,
            Location,
            Rotation,                               // Rotation
            1.0f,                                       // Volume
            1.0f,                                       // Pitch
            0.0f,                                       // Start Time
            AttenuationSettings);           // Attenuation Asset
    }
}

void USoundManager::PlayBeingChased(EChaseState CurrentState)
{
    if (!IsValidAssets())
    {
        UE_LOG(LogTemp, Error, TEXT("USoundManager::PlayingBeingChased(): No AudioAsset"));
        return;
    }

    if (!BeingChasedSource)
    {
        UE_LOG(LogTemp, Error, TEXT("USoundManager::PlayingBeingChased(): No BeingChasedSource"));
        return;
    }

    if (!AudioAssets->BeingChased)
    {
        UE_LOG(LogTemp, Error, TEXT("No BeginChased BGM"));
        return;
    }

 
    BeingChasedSource->SetVolumeMultiplier(1.0f);

    switch (CurrentState)
    {
        case EChaseState::BeingChased:
        {
            if (!BeingChasedSource->IsPlaying())
            {
                BeingChasedSource->FadeIn(0.1f, 1.0f);
            }
            else
            {
                BeingChasedSource->FadeIn(0.4f, 1.0f);
            }
            break;
        }
        case EChaseState::Cooldown:
        {
            BeingChasedSource->FadeOut(1.0f, 0.1f);
            break;
        }
        case EChaseState::Safe:
        {
            if (BeingChasedSource->IsPlaying())
            {
                BeingChasedSource->FadeOut(0.1f, 0.0f);
            }
            break;
        }
        default:
        {
            UE_LOG(LogTemp, Error, TEXT("USoundManager::PlayBeingChased(): Wrong EChaseState"));
            break;
        }
    }
}

void USoundManager::ApplyVolume()
{
    if (!bIsDelegateBound)
    {
        if (IsValid(AudioSettings = UGameAudioSettings::GetGameAudioSettings()))
        {
            AudioSettings->OnAudioSettingsChanged.AddDynamic(this, &USoundManager::ApplyVolume);
        }
    }

    if (!bIsDelegateBound || !IsValid(AudioSettings) || !IsValid(AudioAssets))
    {
        {
            UE_LOG(LogTemp, Warning, TEXT("SoundManager::ApplyVolume(): Delegate not bound or assets are invalid."));
            return;
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("=== 3. SoundManager Received Signal and is Applying Volume! ==="));


    const float MasterVolume = AudioSettings->GetVolume(EAudioCategory::Master);
    const float MusicVolume = AudioSettings->GetVolume(EAudioCategory::Music);
    const float SFXVolume = AudioSettings->GetVolume(EAudioCategory::SFX);
    const float UIVolume = AudioSettings->GetVolume(EAudioCategory::UI);

    UGameplayStatics::SetSoundMixClassOverride(this, AudioAssets->GlobalSoundMix, AudioAssets->MasterSoundClass, MasterVolume, 1.0f, 0.0f);
    UGameplayStatics::SetSoundMixClassOverride(this, AudioAssets->GlobalSoundMix, AudioAssets->MusicSoundClass, MusicVolume, 1.0f, 0.0f);
    UGameplayStatics::SetSoundMixClassOverride(this, AudioAssets->GlobalSoundMix, AudioAssets->SFXSoundClass, SFXVolume, 1.0f, 0.0f);
    UGameplayStatics::SetSoundMixClassOverride(this, AudioAssets->GlobalSoundMix, AudioAssets->UISoundClass, UIVolume, 1.0f, 0.0f);

    UE_LOG(LogTemp, Log, 
        TEXT("Applied volume settings: Master=%.2f, Music=%.2f, SFX=%.2f, UI=%.2f"), 
        MasterVolume, MusicVolume, SFXVolume, UIVolume);
}