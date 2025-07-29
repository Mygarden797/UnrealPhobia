// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/SoundManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"


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