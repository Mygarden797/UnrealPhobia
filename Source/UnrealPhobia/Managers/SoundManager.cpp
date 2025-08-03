// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/SoundManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"

USoundManager::USoundManager()
{
    BeingChased = CreateDefaultSubobject<UAudioComponent>(TEXT("BeingChased"));
    BeingChased->bAutoActivate = false;
}

// 플레이어에게 직접 전달
void USoundManager::PlaySFX2D(USoundBase* SFX)
{
    if (!SFX || !GEngine) return;
    UGameplayStatics::PlaySound2D(GetWorld(), SFX);
    UE_LOG(LogTemp, Display, TEXT("Call PlaySFX2D"));
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

bool USoundManager::IsVaildAssets() const
{
    if (!AudioAssets)
    {
        UE_LOG(LogTemp, Error, TEXT("AudioAssets is NULL!!"));
        return false;
    }
    return true;
}

void USoundManager::PlayBeingChased(EChaseState CurrentState)
{
    if (!IsVaildAssets()) return;

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