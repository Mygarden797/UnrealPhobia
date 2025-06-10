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

    //얘도 임시
    TriggerInfo = new Protocol::ObjectInfo();
    CreatureInfo = new Protocol::ObjectInfo();

}
void ANetworkMirror::SetupMirrorMesh()
{
    //임시
    FString MeshPath = TEXT("/Script/Engine.StaticMesh'/Game/Trigger/StaticMesh/SM_TriggerMirror.SM_TriggerMirror");
    // 메쉬 경로에 해당하는 UStaticMesh 객체 로드
    //FString MeshPath = TEXT("/Game/AI/BP_CreatureGrey.BP_CreatureGrey_C");

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

//소환 위치랑 trigger_id 바꿔야함.
bool ANetworkMirror::ActivateTrigger()
{
    Protocol::C_SPAWN_CREATURE SpawnCreaturePkt;
    TriggerInfo->set_object_id(1);
    {
        auto pos = CreatureInfo->mutable_pos_info();
        pos->set_x(-3972.f);
        pos->set_y(-12010.f);
        pos->set_z(610.f);
        pos->set_yaw(90.f);
    }
    SpawnCreaturePkt.mutable_trigger_info()->CopyFrom(*TriggerInfo);
    SpawnCreaturePkt.mutable_creature_info()->CopyFrom(*CreatureInfo);

    SEND_PACKET(SpawnCreaturePkt);
    return true;

}

bool ANetworkMirror::ActivateTrigger(int64 trigger_id)
{
    Protocol::C_SPAWN_CREATURE SpawnCreaturePkt;
    TriggerInfo->set_object_id(trigger_id);
    {
        auto pos = CreatureInfo->mutable_pos_info();
        pos->set_x(-3972.f);
        pos->set_y(-12010.f);
        pos->set_z(610.f);
        pos->set_yaw(90.f);
    }
    SpawnCreaturePkt.mutable_trigger_info()->CopyFrom(*TriggerInfo);
    SpawnCreaturePkt.mutable_creature_info()->CopyFrom(*CreatureInfo);

    SEND_PACKET(SpawnCreaturePkt);
    return true;
}
