// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/SoundManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"
#include "Misc/ConfigCacheIni.h"

// Initalize Varaiables and Components
USoundManager::USoundManager()
{
    // What the fuck;
}

// Initalize External Resources
void USoundManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    // UE_LOG(LogTemp, Warning, TEXT("USoundManager::Initalize(), should be called only one time"));

    LoadAudioAssets();
    
    if (IsValidAssets() && AudioAssets->GlobalSoundMix)
    {
        UGameplayStatics::PushSoundMixModifier(this, AudioAssets->GlobalSoundMix);
    }
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

// 특정 디렉토리 포함된 에셋 호출
void USoundManager::LoadAudioAssets()
{
    FString AudioAssetPath;
    if (GConfig->GetString(TEXT("GameAudio"), TEXT("DA_AudioAssetPath"),
        AudioAssetPath, GGameIni))
    {
        FSoftObjectPath AudioDataPath(AudioAssetPath);

        // Blueprint로 파생된 AudioAssets를 가져옴
        // 경로는 Config/DefualtGame.ini
        UBlueprint* LoadedBlueprint = Cast<UBlueprint>(AudioDataPath.TryLoad());
        if (LoadedBlueprint)
        {
            AudioAssets = Cast<UAudioAssets>(LoadedBlueprint->GeneratedClass->GetDefaultObject());
        }
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("USoundManager::Initalize(): World is null"));
        return;
    }

    BeingChasedSource = UGameplayStatics::SpawnSound2D(World, AudioAssets->BeingChased);
    if (BeingChasedSource && AudioAssets && AudioAssets->BeingChased)
    {
        BeingChasedSource->bAutoDestroy = false;
        //  BeingChasedSource->bIsUISound = true;
        BeingChasedSource->SetVolumeMultiplier(1.0f);
        UE_LOG(LogTemp, Display, TEXT("USoundManager::Initalize(): Set BeingChasedSource"));
    }
}


// 실제 출력을 위한 오디오 세팅 적용
void USoundManager::ApplyAudioSettings()
{
    if (!IsValidAssets() || !AudioAssets->GlobalSoundMix)
    {
        UE_LOG(LogTemp, Error, TEXT("USoundManager::ApplyAudioSettings(): No GlobalSoundMix"));
        return;
    }

    UGameAudioSettings* Settings = UGameAudioSettings::GetGameAudioSettings();
    if (!Settings)
    {
        UE_LOG(LogTemp, Error, TEXT("USoundManager::ApplyAudioSettings(): Failed to load Settings"));
        return;
    }

    if (AudioAssets->MasterSoundClass)
    {
        UGameplayStatics::SetSoundMixClassOverride(
            // WorldContext, SoundMix* 
            // Volume, Pitch, FadeInTime, bApplyToChildren
            this, AudioAssets->GlobalSoundMix, AudioAssets->MasterSoundClass,
            Settings->GetMasterVolume(), 1.0f, 0.1f, true);    
    }

    if (AudioAssets->MusicSoundClass)
    {
        UGameplayStatics::SetSoundMixClassOverride(
            this, AudioAssets->GlobalSoundMix, AudioAssets->MusicSoundClass,
            Settings->GetMasterVolume(), 1.0f, 0.1f, true);
    }

    if (AudioAssets->SFXSoundClass)
    {
        UGameplayStatics::SetSoundMixClassOverride(
            this, AudioAssets->GlobalSoundMix, AudioAssets->SFXSoundClass,
            Settings->GetMasterVolume(), 1.0f, 0.1f, true);
    }

    if (AudioAssets->UISoundClass)
    {
        UGameplayStatics::SetSoundMixClassOverride(
            this, AudioAssets->GlobalSoundMix, AudioAssets->UISoundClass,
            Settings->GetMasterVolume(), 1.0f, 0.1f, true);
    }
}