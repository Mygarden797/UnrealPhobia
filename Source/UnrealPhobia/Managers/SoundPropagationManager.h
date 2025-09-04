// Copyright © 2025 Unrealphobia. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SoundPropagationManager.generated.h"

/**
*           Name				: SoundPropagationManager
*           Description		: Manage Area that make sounds propagate
*           Last Update	: 2025/09/01
*/

class PropagationZoneActor;

UCLASS()
class UNREALPHOBIA_API USoundPropagationManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

    /*
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

    TMap<APropagationZoneActor*, TSet<AActor*>> ZoneActors;'
    */
};
