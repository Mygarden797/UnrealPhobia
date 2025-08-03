#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ChaseSystemTypes.h"
#include "Sound/SoundBase.h"
#include "UnrealPhobia/Assets/AudioAssets.h"
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
    USoundManager();

    UFUNCTION(BlueprintCallable)
    void PlaySFX2D(USoundBase* SFX);
    UFUNCTION(BlueprintCallable)
    void PlaySFX3D(UObject* Object, USoundBase* SFX, FVector Location, FRotator Rotation, USoundAttenuation* AttenuationSettings);

    void PlayBeingChased(EChaseState CurrentState);

private:
    UPROPERTY()
    UAudioAssets* AudioAssets;

    UAudioComponent* BeingChased;

    FTimerHandle FadeTimerHandle;
    float TargetVolume;
    float FadeDuration;
    float FadeStep;
    float CurrentVolume;

    bool IsVaildAssets() const;
    void UpdateFadeStep();
};