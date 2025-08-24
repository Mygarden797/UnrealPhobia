// Copyright © 2025 Unrealphobia. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SoundPropagationManager.generated.h"

/**
 * 
 */

class PropagationZoneActor;

UCLASS()
class PARANOID_API USoundPropagationManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
    void RegisterZone(APropagationZoneActor* Zone);
    void UnregisterZone(APropagationZoneActor* Zone);

    void NotifyActorEntered(APropagationZoneActor* Zone, AActor* Actor);
    void NotifyActorExited(APropagationZoneActor* Zone, AActor* Actor);

    UFUNCTION(BlueprintCallable, Category = "Sound Propagation")
    APropagationZoneActor* GetZoneOfActor(AActor* Actor) const;

    UFUNCTION(BlueprintCallable, Category = "Sound Propagation")
    void GetActorsInSameZone(AActor* Context, TArray<AActor*>& OutActors) const;

    UFUNCTION(BlueprintCallable)
    void PropagateSoundFrom(AActor* Emitter, float BaseLoudness);

protected:


private:
    UPROPERTY()
    TSet<APropagationZoneActor*> RegisteredZones;

    UPROPERTY()
    TMap<AActor*, APropagationZoneActor*> ActorToZone;

    TMap<APropagationZoneActor*, TSet<AActor*>> ZoneActors;
};
