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

class UAudioAssets;

UCLASS()
class UNREALPHOBIA_API USoundManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
    USoundManager();
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable)
    void PlayDetectedSound();
    UFUNCTION(BlueprintCallable)
    void PlaySFX3D(UObject* Object, USoundBase* SFX, FVector Location, FRotator Rotation, USoundAttenuation* AttenuationSettings);

    void PlayBeingChased(EChaseState CurrentState);

private:
    UPROPERTY()
    UAudioAssets* AudioAssets;

    UAudioComponent* BeingChasedSource;

    FTimerHandle FadeTimerHandle;
    bool IsValidAssets() const;
};