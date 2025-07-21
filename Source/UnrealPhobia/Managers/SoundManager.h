// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Sound/SoundBase.h"
#include "SoundManager.generated.h"

/**
 * 
 */
UCLASS()
class UNREALPHOBIA_API USoundManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void PlaySFX2D(USoundBase* SFX);
    UFUNCTION(BlueprintCallable)
    void PlaySFX3D(USoundBase* SFX, FVector Location);

private:
    UPROPERTY()
    UAudioComponent* CurrentBGMComponent;
    
};
