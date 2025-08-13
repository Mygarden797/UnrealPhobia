// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Trigger/Trigger.h"
#include "Trigger/TriggerInventory.h"
#include "Trigger/TriggerSpawnPoint.h"
#include "Protocol.pb.h"
#include "GameFramework/GameStateBase.h"
#include "ProtoGameState.generated.h"

/*
 - Name        : AProtoGameState
 - Description : 맵의 상태를 저장하는 클래스 -> 저장할 필요가 있나...?
*/

UCLASS()
class UNREALPHOBIA_API AProtoGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
    AProtoGameState();

    //const FString CreatureGrey = TEXT("/Game/AI/BP_CreatureGrey.BP_CreatureGrey_C"); //초기 위치 표현
    //const FString CreatureGhost = TEXT("/Game/AI/BP_CreatureGhost.BP_CreatureGhost_C"); //초기 위치 표현

    const FString CreatureGrey = TEXT("/Game/Creature/AI/BP_CreatureGrey.BP_CreatureGrey_C"); //초기 위치 표현
    const FString CreatureGhost = TEXT("/Game/Creature/AI/BP_Ghost.BP_Ghost_C"); //초기 위치 표현

    bool SpawnTrigger(Protocol::ObjectInfo);
    
    bool SpawnCreature(FVector SpawnPoint, FRotator SpawnRotate, Protocol::CreatureType Creature, bool bAIControl);

};