// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/ProtoGameState.h"
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
        int32 RandValue = FMath::RandRange(0, static_cast<int32>(ETriggerName::None) - 1);
        SpawnedTrigger->SetTriggerName(static_cast<ETriggerName>(RandValue));
        return true;
    }
    return false;
}
