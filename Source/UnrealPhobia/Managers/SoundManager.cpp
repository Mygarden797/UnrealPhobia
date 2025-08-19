// Fill out your copyright notice in the Description page of Project Settings.

#include "Managers/SoundManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"
#include "Misc/ConfigCacheIni.h"

// Initalize External Resources
void USoundManager::Initialize(FSubsystemCollectionBase &Collection)
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
        // UE_LOG(LogTemp, Warning, TEXT("=== Global Sound Mix Activated! ==="));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("USoundManager::Initalize(): Failed to get AudioAssets or GlobalSoundMix"));
    }

    ApplyVolume();
}

void USoundManager::Deinitialize()
{
    if (AudioSettings)
    {
        AudioSettings->OnAudioSettingsChanged.RemoveDynamic(this, &USoundManager::ApplyVolume);
    }
    Super::Deinitialize();
}

void USoundManager::Tick(float DeltaTime)
{
    /*     if (GEngine)
        {
            // 1. AudioAssets 데이터 에셋 자체의 유효성 검사
            if (IsValid(AudioAssets))
            {
                GEngine->AddOnScreenDebugMessage(3, 0.f, FColor::Green, TEXT("SoundManager: AudioAssets is Valid"));

                if (IsValid(AudioAssets->BeingChased))
                {
                    GEngine->AddOnScreenDebugMessage(4, 0.f, FColor::Green, TEXT("SoundManager: AudioAssets->BeingChased is Valid"));
                }
                else
                {
                    GEngine->AddOnScreenDebugMessage(4, 0.f, FColor::Red, TEXT("SoundManager: AudioAssets->BeingChased is NOT Valid"));
                }
            }
            else
            {
                GEngine->AddOnScreenDebugMessage(3, 0.f, FColor::Red, TEXT("SoundManager: AudioAssets is NOT Valid"));
            }

            if (IsValid(BeingChasedSource))
            {
                GEngine->AddOnScreenDebugMessage(5, 0.f, FColor::Green, TEXT("SoundManager: BeingChaseSource is Valid"));
            }
            else
            {
                GEngine->AddOnScreenDebugMessage(5, 0.f, FColor::Red, TEXT("SoundManager: BeingChaseSource is NOT Valid"));
            }
        } */
}

// [5] Tickable 시스템이 통계 정보를 수집하기 위해 필요한 함수입니다. 아래와 같이 그대로 작성하면 됩니다.
TStatId USoundManager::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(USoundManager, STATGROUP_Tickables);
}

bool USoundManager::IsValidAssets() const
{
    if (!IsValid(AudioAssets))
    {
        UE_LOG(LogTemp, Error, TEXT("USoundManager: AudioAssets is NOT valid!!"));
        return false;
    }
    return true;
}

// 플레이어에게 직접 전달
void USoundManager::PlayDetectedSound()
{
    if (IsValidAssets())
    {
        if (IsValid(AudioAssets->BeDetected))
        {
            UGameplayStatics::PlaySound2D(GetWorld(), AudioAssets->BeDetected);
            // UE_LOG(LogTemp, Display, TEXT("USoundManager::PlayDetectedSound()"));
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
        if (IsValid(AudioAssets->BeDetected))
        {
            UGameplayStatics::PlaySound2D(GetWorld(), AudioAssets->BeDetected);
            // UE_LOG(LogTemp, Display, TEXT("USoundManager::PlayDetectedSound()"));
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
void USoundManager::PlaySFX3D(UObject *Object, USoundBase *SFX, FVector Location, FRotator Rotation, USoundAttenuation *AttenuationSettings)
{
    if (!SFX)
        return;
    else
    {
        UGameplayStatics::PlaySoundAtLocation(
            Object,
            SFX,
            Location,
            Rotation,             // Rotation
            1.0f,                 // Volume
            1.0f,                 // Pitch
            0.0f,                 // Start Time
            AttenuationSettings); // Attenuation Asset
    }
}

void USoundManager::PlayBeingChased(EChaseState CurrentState)
{
    if (!IsValidAssets())
    {
        UE_LOG(LogTemp, Error, TEXT("USoundManager::PlayingBeingChased(): No AudioAsset"));
        return;
    }

    if (!AudioAssets->BeingChased)
    {
        UE_LOG(LogTemp, Error, TEXT("No BeginChased BGM"));
        return;
    }

    if (!IsValid(BeingChasedSource))
    {
        // UE_LOG(LogTemp, Warning, TEXT("USoundManager::PlayingBeingChased():  BeingChasedSource is not valid, Initalizing..."));
        BeingChasedSource = UGameplayStatics::SpawnSound2D(GetWorld(), AudioAssets->BeingChased);
        BeingChasedSource->bAutoDestroy = false;
    }

    switch (CurrentState)
    {
    case EChaseState::BeingChased:
    {
        if (!(BeingChasedSource->IsPlaying()))
        {
            BeingChasedSource->FadeIn(0.1f, 1.0f);
        }
        else
        {
            BeingChasedSource->FadeIn(0.4f, 1.0f);
        }
        // UE_LOG(LogTemp, Warning, TEXT("USoundManager::PlayingBeingChased(): Fade In"));
        break;
    }
    case EChaseState::Cooldown:
    {
        BeingChasedSource->FadeOut(1.0f, 0.1f);
        // UE_LOG(LogTemp, Warning, TEXT("USoundManager::PlayingBeingChased(): Fade Out"));
        break;
    }
    case EChaseState::Safe:
    {
        if (BeingChasedSource->IsPlaying())
        {
            BeingChasedSource->FadeOut(0.1f, 0.0f);
        }
        BeingChasedSource = nullptr;
        // UE_LOG(LogTemp, Warning, TEXT("USoundManager::PlayingBeingChased(): Stop"));
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
    // UE_LOG(LogTemp, Warning, TEXT("=== 3. SoundManager Received Signal and is Applying Volume! ==="));

    const float MasterVolume = AudioSettings->GetVolume(EAudioCategory::Master);
    const float MusicVolume = AudioSettings->GetVolume(EAudioCategory::Music);
    const float SFXVolume = AudioSettings->GetVolume(EAudioCategory::SFX);
    const float UIVolume = AudioSettings->GetVolume(EAudioCategory::UI);

    UGameplayStatics::SetSoundMixClassOverride(this, AudioAssets->GlobalSoundMix, AudioAssets->MasterSoundClass, MasterVolume, 1.0f, 0.0f);
    UGameplayStatics::SetSoundMixClassOverride(this, AudioAssets->GlobalSoundMix, AudioAssets->MusicSoundClass, MusicVolume, 1.0f, 0.0f);
    UGameplayStatics::SetSoundMixClassOverride(this, AudioAssets->GlobalSoundMix, AudioAssets->SFXSoundClass, SFXVolume, 1.0f, 0.0f);
    UGameplayStatics::SetSoundMixClassOverride(this, AudioAssets->GlobalSoundMix, AudioAssets->UISoundClass, UIVolume, 1.0f, 0.0f);

    // UE_LOG(LogTemp, Log,
    //    TEXT("Applied volume settings: Master=%.2f, Music=%.2f, SFX=%.2f, UI=%.2f"),
    //    MasterVolume, MusicVolume, SFXVolume, UIVolume);
}