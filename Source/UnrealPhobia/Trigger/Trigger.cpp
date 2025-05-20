#include "Trigger.h"
#include "Components/StaticMeshComponent.h"

ATrigger::ATrigger()
{
	PrimaryActorTick.bCanEverTick = true;
	// Create components
	BaseMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMeshComponent->SetCollisionResponseToChannel(
		ECC_GameTraceChannel1,		  // Sweep 채널
		ECollisionResponse::ECR_Block // Block되도록
	);
	BaseMeshComponent->SetCollisionResponseToChannel(
		ECollisionChannel::ECC_Pawn,   // Pawn 채널
		ECollisionResponse::ECR_Ignore // Ignore 되도록
	);
}

void ATrigger::SetTriggerName(ETriggerName Name)
{
	TriggerName = Name;
	SetupTriggerMesh();
}

void ATrigger::SetupTriggerMesh()
{
	// enum 값에 따라 사용할 에셋 경로를 매핑
	FString MeshPath;
	switch (TriggerName)
	{
	case ETriggerName::Grey:
		MeshPath = TEXT("/Script/Engine.StaticMesh'/Game/Trigger/StaticMesh/SM_Trigger1.SM_Trigger1'");
		break;
	case ETriggerName::Ghost:
		MeshPath = TEXT("/Script/Engine.StaticMesh'/Game/Trigger/StaticMesh/SM_Trigger2.SM_Trigger2'");
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("Unknown TriggerName, skipping mesh load"));
		return;
	}
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
		UE_LOG(LogTemp, Log, TEXT("Loaded mesh for Trigger '%s' from %s"),
			   *UEnum::GetValueAsString(TriggerName),
			   *MeshPath);
	}
	else
	{
		// 에러 로그 출력
		UE_LOG(LogTemp, Error, TEXT("Failed to load mesh at %s"), *MeshPath);
	}
}

void ATrigger::BeginPlay()
{
	Super::BeginPlay();
	SetupTriggerMesh();
	BaseMeshComponent->SetSimulatePhysics(true);
	BaseMeshComponent->SetMassOverrideInKg(NAME_None, 500.0f);
}

void ATrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}