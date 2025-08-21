// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/ProtoGameState.h"
#include "Creature/CreatureBase.h"
#include "Trigger/Trigger.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"

AProtoGameState::AProtoGameState()
{
    UE_LOG(LogTemp, Log, TEXT("GameState Started"));
}


//그냥 소환해 버려
bool AProtoGameState::SpawnTrigger(Protocol::ObjectInfo objectInfo)
{
    Protocol::PosInfo pos = objectInfo.pos_info();
    const FVector SpawnLocation = FVector(pos.x(), pos.y(), pos.z());
    const FRotator SpawnRotation = FRotator(0.0f, pos.yaw(), 0.0f);

    // 트리거 스폰
    // ObjectId 포함하게 해서 관리하는게 좋을 듯
    FActorSpawnParameters Params;
    ATrigger* SpawnedTrigger = GetWorld()->SpawnActor<ATrigger>(
        ATrigger::StaticClass(),
        SpawnLocation,
        SpawnRotation,
        Params);
    if (SpawnedTrigger)
    {
        // 스폰된 트리거에 트리거 종류 랜덤으로 부여
        int32 RandValue = FMath::RandRange(static_cast<int32>(ETriggerName::None) + 1, 2);
        SpawnedTrigger->SetTriggerName(static_cast<ETriggerName>(RandValue));
        SpawnedTrigger->TriggerInfo->set_object_id(objectInfo.object_id());
        return true;
    }
    return false;
}

bool AProtoGameState::SpawnCreature(uint64 ObjectId, FVector SpawnPoint, FRotator SpawnRotate, Protocol::CreatureType Creature, bool bAIControl)
{
    FString CreaturePath;
    if (Creature == Protocol::CreatureType::CREATURE_TYPE_NONE)
    {
        UE_LOG(LogTemp, Warning, TEXT("CreatureType is NONE"));
        return false;
    }
    else if (Creature == Protocol::CreatureType::CREATURE_TYPE_GREY)
    {
        CreaturePath = CreatureGrey;
    }
    else if (Creature == Protocol::CreatureType::CREATURE_TYPE_GHOST)
    {
        CreaturePath = CreatureGhost;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Unknown CreatureType"));
        return false;
    }

    TSubclassOf<ACreatureBase> BP_Creature;
    BP_Creature = LoadClass<ACreatureBase>(nullptr, *CreaturePath);
    if (BP_Creature == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("BP_Creature is not Load"));
        return false;
    }
    FActorSpawnParameters Params;

    ACreatureBase* SpawnActor;

    //ACreatureBase* SpawnActor = GetWorld()->SpawnActor<ACreatureBase>(ACreatureBase::StaticClass(), SpawnPoint, SpawnRotate, Params);

    SpawnActor = GetWorld()->SpawnActor<ACreatureBase>(BP_Creature, SpawnPoint, SpawnRotate, Params);

    if (bAIControl == false)
    {
        //이거 왜 되지?
        //SpawnActor = GetWorld()->SpawnActor<ACreatureBase>(BP_Creature, SpawnPoint, SpawnRotate, Params);
 /*       if (SpawnActor->GetController())
        {
            SpawnActor->GetController()->UnPossess();
        }*/


        //// 자동 AI 소유 비활성화
        //SpawnActor->AutoPossessAI = EAutoPossessAI::Disabled;
        //SpawnActor->AIControllerClass = nullptr;
        //SpawnActor->bHaveAIController = false;
        // 자동 AI 소유 비활성화하되, 기본 AIController는 유지
        SpawnActor->AutoPossessAI = EAutoPossessAI::Disabled;
        SpawnActor->AIControllerClass = nullptr;
        //SpawnActor->AIControllerClass = AAIController::StaticClass();
        SpawnActor->bHaveAIController = false; 
    }



    SpawnActor->SetObjectId(ObjectId);
    
    if (auto* GameInstance = Cast<UNetworkManager>(GWorld->GetGameInstance()))
    {
        GameInstance->Creatures.Add(ObjectId, SpawnActor);
    }
    return true;
}