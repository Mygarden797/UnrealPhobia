// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/SoundManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"
#include "Misc/ConfigCacheIni.h"

USoundManager::USoundManager()
{
    BeingChased = CreateDefaultSubobject<UAudioComponent>(TEXT("BeingChased"));
    BeingChased->bAutoActivate = false;

    if (IsValidAssets())
    {
        UE_LOG(LogTemp, Display, TEXT("AudioAssets is ready"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("USoundManager::Constructor; AudioAssets is null"));
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

void USoundManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    // UE_LOG(LogTemp, Warning, TEXT("USoundManager::Initalize(), should be called only one time"));
    FString AudioAssetPath;
    if (GConfig->GetString(TEXT("GameAudio"), TEXT("DA_AudioAssetPath"),
        AudioAssetPath, GGameIni))
    {
        FSoftObjectPath AudioDataPath(AudioAssetPath);
        UBlueprint* LoadedBlueprint = Cast<UBlueprint>(AudioDataPath.TryLoad());
        if (LoadedBlueprint)
        {
            AudioAssets = Cast<UAudioAssets>(LoadedBlueprint->GeneratedClass->GetDefaultObject());
        }
    }

    if (IsValidAssets())
    {
        UE_LOG(LogTemp, Log, TEXT("USoundManager::Initialize; AudioAssets is ready"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Check the .ini file"));
    }
}

void USoundManager::Deinitialize()
{
    UE_LOG(LogTemp, Error, TEXT("USoundManager DEINITIALIZED!! This should only happen when the game is closing."));
    Super::Deinitialize();
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
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("USoundManager::PlayDetectedSound(): No Assets"));
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
    if (!IsValidAssets()) return;

    if (!AudioAssets->BeingChased)
    {
        UE_LOG(LogTemp, Error, TEXT("No BeginChased BGM"));
        return;
    }

    BeingChased->SetSound(AudioAssets->BeingChased);
    switch (CurrentState)
    {
        case EChaseState::BeingChased:
        {
            if (!BeingChased->IsPlaying())
            {
                BeingChased->Play();
            }
            TargetVolume = 1.0f;
            FadeDuration = 2.0f;
            break;
        }
        case EChaseState::Cooldown:
        {
            TargetVolume = 0.3f;
            FadeDuration = 2.0f;
            break;
           }
        case EChaseState::Safe:
        {
            TargetVolume = 0.0f;
            FadeDuration = 2.0f;
            break;
        }
        const float Interval = 0.1f;
        FadeStep = (TargetVolume - CurrentVolume) / (FadeDuration / Interval);
        GetWorld()->GetTimerManager().SetTimer(
            FadeTimerHandle, 
            this, 
            &USoundManager::UpdateFadeStep,
            Interval,
            true);
    }
}

void USoundManager::UpdateFadeStep()
{
    CurrentVolume += FadeStep;

    bool bReachedTarget =
        (FadeStep > 0 && CurrentVolume >= TargetVolume) || 
        (FadeStep < 0 && CurrentVolume <= TargetVolume);

    if (bReachedTarget)
    {
        CurrentVolume = TargetVolume;
        GetWorld()->GetTimerManager().ClearTimer(FadeTimerHandle);

        if (FMath::IsNearlyZero(CurrentVolume))
        {
            BeingChased->Stop();
        }
    }

    BeingChased->SetVolumeMultiplier(CurrentVolume);
}