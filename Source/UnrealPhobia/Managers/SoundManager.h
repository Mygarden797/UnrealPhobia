#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "Types/ChaseSystemTypes.h"
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
class UNREALPHOBIA_API USoundManager : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    virtual void Tick(float DeltaTime) override;
    virtual  TStatId GetStatId() const override;
    virtual bool IsTickable() const override { return true; }

    /* Play Audio Sources */
    void PlayDetectedSound(AActor* Actor);
    void PlayBeingChased(AActor* Actor, EChaseState CurrentState);
    void PlayScream();
    // void PlaySurvivorSFX(AActor* Actor, int32 Num);

    /* Sound Mix Settings */
    UFUNCTION()
    void ApplyVolume();

private:
    UPROPERTY()
    TObjectPtr<UAudioAssets> AudioAssets;

    UPROPERTY()
    TObjectPtr<UGameAudioSettings> AudioSettings;

    UPROPERTY()
    TObjectPtr<UAudioComponent> BeingChasedSource;
    UPROPERTY()
    TObjectPtr<AActor> AudioActor;

    bool IsValidAssets() const;
    bool bIsDelegateBound = false;
};