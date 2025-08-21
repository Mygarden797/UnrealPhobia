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
 - Description : ���� ���¸� �����ϴ� Ŭ���� -> ������ �ʿ䰡 �ֳ�...?
*/

UCLASS()
class UNREALPHOBIA_API AProtoGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
    AProtoGameState();

    //const FString CreatureGrey = TEXT("/Game/AI/BP_CreatureGrey.BP_CreatureGrey_C"); //�ʱ� ��ġ ǥ��
    //const FString CreatureGhost = TEXT("/Game/AI/BP_CreatureGhost.BP_CreatureGhost_C"); //�ʱ� ��ġ ǥ��

    const FString CreatureGrey = TEXT("/Game/Creature/Blueprints/BP_CreatureGrey.BP_CreatureGrey_C"); //�ʱ� ��ġ ǥ��
    const FString CreatureGhost = TEXT("/Game/Creature/Blueprints/BP_Ghost.BP_Ghost_C"); //�ʱ� ��ġ ǥ��

    bool SpawnTrigger(Protocol::ObjectInfo);
    
    bool SpawnCreature(uint64 ObjectId, FVector SpawnPoint, FRotator SpawnRotate, Protocol::CreatureType Creature, bool bAIControl);

};