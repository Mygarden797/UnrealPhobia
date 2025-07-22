// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/Contents/NetworkMirror.h"
#include "Trigger/TriggerSpawnManager.h"
#include "NetworkBase.h"

ANetworkMirror::ANetworkMirror()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    BaseMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
    BaseMeshComponent->SetCollisionResponseToChannel(
        ECC_GameTraceChannel1,		  // Sweep 채널
        ECollisionResponse::ECR_Block // Block되도록
    );

}
void ANetworkMirror::SetupMirrorMesh()
{
    //임시
    FString MeshPath = TEXT("/Script/Engine.StaticMesh'/Game/Trigger/StaticMesh/SM_TriggerMirror.SM_TriggerMirror");
    // 메쉬 경로에 해당하는 UStaticMesh 객체 로드

    UStaticMesh* LoadedMesh = Cast<UStaticMesh>(StaticLoadObject(
        UStaticMesh::StaticClass(), // 로드할 객체의 클래스 타입 지정 (UStaticMesh)
        nullptr,					// 패키지 지정 (nullptr이면 기본 패키지 사용)
        *MeshPath					// 로드할 에셋의 경로 (FString → TCHAR* 변환)
    ));

    // 로드가 성공여부 확인
    if (LoadedMesh)
    {
        // 로드된 메쉬를 컴포넌트에 설정
        BaseMeshComponent->SetStaticMesh(LoadedMesh);
        // 메시 경로를 포함한 로드 성공 로그 출력
        UE_LOG(LogTemp, Log, TEXT("Loaded mesh for Trigger Mirror from %s"),
            *MeshPath);
    }
    else
    {
        // 에러 로그 출력
        UE_LOG(LogTemp, Error, TEXT("Failed to load mesh at %s"), *MeshPath);
    }
}
// Called when the game starts or when spawned
void ANetworkMirror::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void ANetworkMirror::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

bool ANetworkMirror::ActivateTrigger(int64 trigger_id)
{
    Protocol::C_SPAWN_CREATURE SpawnCreaturePkt;

    class Protocol::ObjectInfo* TriggerInfo = SpawnCreaturePkt.mutable_trigger_info();
    class Protocol::ObjectInfo* CreatureInfo = SpawnCreaturePkt.mutable_creature_info();
    class Protocol::CreatureInfo* CreatureInfo_Spawn = CreatureInfo->mutable_creature_info();
    TriggerInfo->set_object_id(trigger_id);


    //거울 location_id보고  해당 위치에 소환.
    FString LocationIdStr = location_id.ToString();
    int64 location_id_int = FCString::Atoi64(*LocationIdStr);
    CreatureInfo_Spawn->set_spawn_point(location_id_int);

    SEND_PACKET(SpawnCreaturePkt);
    return true;
}
