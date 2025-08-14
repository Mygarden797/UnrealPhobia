#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Types/ChaseSystemTypes.h"
#include "Sound/SoundBase.h"
#include "Assets/AudioAssets.h"
#include "Settings/GameAudioSettings.h"
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
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /* Play Audio Sources */
    UFUNCTION(BlueprintCallable)
    void PlayDetectedSound();
    UFUNCTION(BlueprintCallable)
    void PlaySFX3D(UObject* Object, USoundBase* SFX, FVector Location, FRotator Rotation, USoundAttenuation* AttenuationSettings);
    void PlayBeingChased(EChaseState CurrentState);
    void PlayScream();

    /* Sound Mix Settings */
    UFUNCTION()
    void ApplyVolume();

private:
    UPROPERTY()
    TObjectPtr<UAudioAssets> AudioAssets;
    TObjectPtr<UGameAudioSettings> AudioSettings;

    UAudioComponent* BeingChasedSource;

    bool IsValidAssets() const;
    bool bIsDelegateBound = false;
};