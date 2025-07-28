// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Sound/SoundBase.h"
#include "SoundManager.generated.h"

/*
    - Name				: SoundManager
    - Description		: Manage playing sounds
    - Date				    : 2025/07/28
*/
UCLASS()
class UNREALPHOBIA_API USoundManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void PlaySFX2D(USoundBase* SFX);
    UFUNCTION(BlueprintCallable)
    void PlaySFX3D(UObject* Object, USoundBase* SFX, FVector Location, FRotator Rotation, USoundAttenuation* AttenuationSettings);

    //UFUNCTION(BlueprintCallable)
    //void PlayChasingBGM();

private:
    UPROPERTY(EditAnywhere, Category = "Audio")
    UAudioComponent* ChasingBGM;
    
};
