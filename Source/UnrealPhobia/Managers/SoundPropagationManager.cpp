// Copyright © 2025 Unrealphobia. All Rights Reserved.


#include "Managers/SoundPropagationManager.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include"Level/PropagationZoneActor.h"
#include "Perception/AISense_Hearing.h"
#include "DrawDebugHelpers.h"

/*
void USoundPropagationManager::RegisterZone(APropagationZoneActor* Zone)
{
    if (!Zone)
    {
        UE_LOG(LogTemp, Error, TEXT("USoundPropagationManager::RegisterZone(): No PropagationZone"));
        return;
    }

    UE_LOG(LogTemp, Display, TEXT("USoundPropagationManager::RegisterZone(): Add a Zone %s in RegisteredZone"), *GetNameSafe(Zone));
    RegisteredZones.Add(Zone);

    // 해당 Zone에 Actor Set을 반드시 초기화
    UE_LOG(LogTemp, Display, TEXT("USoundPropagationManager::RegisterZone(): Add a Zone %s in ZoneActorsZone"), *GetNameSafe(Zone));
    ZoneActors.FindOrAdd(Zone);

    // Zone 내부에 이미 있던 Actor들 처리
    for (AActor* Actor : Zone->RegisteredActors)
    {
        if (IsValid(Actor))
        {
            if (Actor->IsA(APropagationZoneActor::StaticClass()))
            {
                UE_LOG(LogTemp, Warning, TEXT("USoundPropagationManager::RegisterZone(): PrpagationZoneActor is excepted"));
                continue;
            }
            ActorToZone.Add(Actor, Zone);
            UE_LOG(LogTemp, Display, TEXT("USoundPropagationManager::RegisterZone(): Add an Actor %s in ActorToZone"), *GetNameSafe(Actor));
            ZoneActors[Zone].Add(Actor);
            UE_LOG(LogTemp, Display, TEXT("USoundPropagationManager::RegisterZone(): Add an Actor %s in ZoneActors"), *GetNameSafe(Actor));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("USoundPropagationManager::RegisterPropagationZone: Invalid Actor in Zone"));
        }
    }
}

void USoundPropagationManager::UnregisterZone(APropagationZoneActor* Zone)
{
    if (!Zone)
    {
        UE_LOG(LogTemp, Error, TEXT("USoundPropagationManager::UnregisterPropagationZone(): No PropagationZone"));
        return;
    }

    if (TSet<AActor*>* SetPtr = ZoneActors.Find(Zone))
    {
        for (AActor* Actor : *SetPtr)
        {
            if (IsValid(Actor))
            {
                ActorToZone.Remove(Actor);
                UE_LOG(LogTemp, Display, TEXT("USoundPropagationManager::UnregisterZone(): Remove an Actor %s in ActorToZone"), *GetNameSafe(Actor));
            }
        }
    }

    ZoneActors.Remove(Zone);
    UE_LOG(LogTemp, Display, TEXT("USoundPropagationManager::UnregisterZone(): Remove a Zone %s in ZoneActors"), *GetNameSafe(Zone));
    RegisteredZones.Remove(Zone);
    UE_LOG(LogTemp, Display, TEXT("USoundPropagationManager::RegisterZone(): Remove a Zone %s in RegisteredZones"), *GetNameSafe(Zone));
}

void USoundPropagationManager::NotifyActorEntered(APropagationZoneActor* Zone, AActor* Actor)
{
    if (!Zone)
    {
        UE_LOG(LogTemp, Error, TEXT("USoundPropagationManager::NotifyActorEntered(): No Zone"));
        return;
    }
    if (!IsValid(Actor))
    {
        UE_LOG(LogTemp, Error, TEXT("USoundPropagationManager::NotifyActorEntered(): Invalid Actor"));
        return;
    }

    ActorToZone.Add(Actor, Zone);
    ZoneActors.FindOrAdd(Zone).Add(Actor);
}

void USoundPropagationManager::NotifyActorExited(APropagationZoneActor* Zone, AActor* Actor)
{
    if (!Zone)
    {
        UE_LOG(LogTemp, Error, TEXT("USoundPropagationManager::NotifyActorExited(): No Zone"));
        return;
    }
    if (!IsValid(Actor))
    {
        UE_LOG(LogTemp, Error, TEXT("USoundPropagationManager::NotifyActorExited(): Invalid Actor"));
        return;
    }

    ActorToZone.Remove(Actor);
    if (TSet<AActor*>* SetPtr = ZoneActors.Find(Zone))
    {
        SetPtr->Remove(Actor);
    }
}

APropagationZoneActor* USoundPropagationManager::GetZoneOfActor(AActor* Actor) const
{
    if (!IsValid(Actor))
    {
        UE_LOG(LogTemp, Error, TEXT("USoundPropagationManager::GetZoneOfActor(): Invalid Actor"));
        return nullptr;
    }

    if (APropagationZoneActor* const* ZonePtr = ActorToZone.Find(Actor))
    {
        return *ZonePtr;
    }
    else
    {
        for (APropagationZoneActor* Zone : RegisteredZones)
        {
            if (IsValid(Zone) && Zone->ContainsLocation(Actor->GetActorLocation()))
            {
                return Zone;
            }
        }
    }

    return nullptr;
}

void USoundPropagationManager::GetActorsInSameZone(AActor* Context, TArray<AActor*>& OutActors) const
{
    OutActors.Reset();

    APropagationZoneActor* Zone = GetZoneOfActor(Context);
    if (!IsValid(Zone))
    {
        UE_LOG(LogTemp, Warning, TEXT("USoundPropagationManager::GetActorsInSameZone(): Invalid Zone"));
        return;
    }

    if (const TSet<AActor*>* SetPtr = ZoneActors.Find(Zone))
    {
        for (AActor* Actor : *SetPtr)
        {
            if (IsValid(Actor))
            {
                OutActors.Add(Actor);
            }
        }
    }
}

void USoundPropagationManager::PropagateSoundFrom(AActor* Emitter, float BaseLoudness)
{
    if (!IsValid(Emitter))
    {
        UE_LOG(LogTemp, Error, TEXT("USoundPropagationManager::PropagateSoundFrom(): Invalid Emitter"));
        return;
    }

    APropagationZoneActor* Zone = GetZoneOfActor(Emitter);
    if (!IsValid(Zone))
    {
        UE_LOG(LogTemp, Warning, TEXT("USoundPropagationManager::PropagateSoundFrom(): Emitter not in any Zone"));
        return;
    }

    if (const TSet<AActor*>* SetPtr = ZoneActors.Find(Zone))
    {
        for (AActor* Listener : *SetPtr)
        {
            if (!IsValid(Listener) || Listener == Emitter)
                continue;

            // TODO: 여기서 리버브/오클루전/감쇠 적용 처리
        }
    }
}
*/