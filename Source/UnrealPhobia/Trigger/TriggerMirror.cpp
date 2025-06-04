// Fill out your copyright notice in the Description page of Project Settings.
#include "Trigger/TriggerMirror.h"
#include "TriggerSpawnManager.h"

// Sets default values
ATriggerMirror::ATriggerMirror()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BaseMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMeshComponent->SetCollisionResponseToChannel(
		ECC_GameTraceChannel1,		  // Sweep 채널
		ECollisionResponse::ECR_Block // Block되도록
	);
}
void ATriggerMirror::SetupMirrorMesh()
{
	FString MeshPath = TEXT("/Script/Engine.StaticMesh'/Game/Trigger/StaticMesh/SM_TriggerMirror.SM_TriggerMirror");
	// 메쉬 경로에 해당하는 UStaticMesh 객체 로드

	UStaticMesh *LoadedMesh = Cast<UStaticMesh>(StaticLoadObject(
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
void ATriggerMirror::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATriggerMirror::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ATriggerMirror::ActivateTrigger(ETriggerName TriggerName)
{
	if (!bCanActivate)
	{
		UE_LOG(LogTemp, Log, TEXT("Can not Activate Trigger"));
		return false;
	}
	UE_LOG(LogTemp, Log, TEXT("Trigger Activated"));
	StartCooldown();
	APlayerController *PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		ATriggerSpawnManager *TSM = Cast<ATriggerSpawnManager>(PC->PlayerState);

		// Respawn trigger
		if (TSM)
		{
			TSM->StartSpawn();
		}
	}
	return true;
}
void ATriggerMirror::StartCooldown()
{
	bCanActivate = false;
	UE_LOG(LogTemp, Log, TEXT("Start Cooldown"));
	GetWorld()->GetTimerManager().SetTimer(
		CooldownTimerHandle,		  // 타이머 핸들
		this,						  // 호출 대상
		&ATriggerMirror::EndCooldown, // 호출할 함수
		ActivateCoolTime,			  // 지연 시간 (초)
		false						  // 반복 여부
	);
}
void ATriggerMirror::EndCooldown()
{
	bCanActivate = true;
	UE_LOG(LogTemp, Log, TEXT("End Cooldown"));
}