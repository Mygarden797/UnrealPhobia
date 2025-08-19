#include "Trigger.h"
#include "Components/StaticMeshComponent.h"
#include "NetworkPlayer.h"
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

	TriggerInfo = new Protocol::ObjectInfo();
}

ATrigger::~ATrigger()
{
	delete TriggerInfo;
	TriggerInfo = nullptr;
}

// 트리거 종류 설정 및 메시 재적용
void ATrigger::SetTriggerName(ETriggerName Name)
{
	TriggerName = Name;
	SetupTriggerMesh();
}

void ATrigger::SetupTriggerMesh()
{
	// enum 값에 따라 사용할 에셋 경로를 매핑
	FString MeshPath, MaterialPath = TEXT("/Script/Engine.Material'/Game/Trigger/Materials/M_Highlight.M_Highlight'");
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
	TriggerMesh = Cast<UStaticMesh>(StaticLoadObject(
		UStaticMesh::StaticClass(), // 로드할 객체의 클래스 타입 지정 (UStaticMesh)
		nullptr,					// 패키지 지정
		*MeshPath					// 로드할 에셋의 경로
		));

	if (TriggerMesh)
	{
		BaseMeshComponent->SetStaticMesh(TriggerMesh);					   // 로드된 메쉬를 컴포넌트에 설정
		UE_LOG(LogTemp, Log, TEXT("Loaded mesh for Trigger '%s' from %s"), // 메시 경로를 포함한 로드 성공 로그 출력
			   *UEnum::GetValueAsString(TriggerName),
			   *MeshPath);
	}
	else
	{
		// 에러 로그 출력
		UE_LOG(LogTemp, Error, TEXT("Failed to load mesh at %s"), *MeshPath);
	}

	HighlightMaterial = Cast<UMaterial>(StaticLoadObject(
		UMaterial::StaticClass(), // 로드할 객체의 클래스 타입 지정 (UMaterial)
		nullptr,				  // 패키지 지정
		*MaterialPath			  // 로드할 에셋의 경로
		));

	if (HighlightMaterial)
	{
		UE_LOG(LogTemp, Log, TEXT("Loaded material from %s"), // 머티리얼 경로를 포함한 로드 성공 로그 출력,
			   *MaterialPath);
	}
	else
	{
		// 에러 로그 출력
		UE_LOG(LogTemp, Error, TEXT("Failed to load material at %s"), *MaterialPath);
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
	if (CheckHighlight())
	{
		if (!bIsHighlighting)
		{
			bIsHighlighting = true;
			BaseMeshComponent->SetOverlayMaterial(HighlightMaterial);
			UE_LOG(LogTemp, Log, TEXT("Highlighting On"));
		}
	}
	else
	{
		if (bIsHighlighting)
		{
			bIsHighlighting = false;
			BaseMeshComponent->SetOverlayMaterial(nullptr);
			UE_LOG(LogTemp, Log, TEXT("Highlighting Off"));
		}
	}
}

bool ATrigger::CheckHighlight()
{
	TriggerLocation = GetActorLocation();
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn); // 검사할 오브젝트 타입 : Pawn

	FVector BoxHalfExtent = FVector(230.0f, 230.0f, 80.0f); // 박스 형태 충돌 범위
	FCollisionShape CollisionShape = FCollisionShape::MakeBox(BoxHalfExtent);

	// 쿼리 파라미터 : 자기 자신 무시
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	TArray<FOverlapResult> OverlapResults; // 결과 저장용

	bool bOverlap = GetWorld()->OverlapMultiByObjectType( // Overlap 검사
		OverlapResults,
		TriggerLocation,
		FQuat::Identity,
		ObjectQueryParams,
		CollisionShape,
		QueryParams);

	// 디버그 시각화: 박스 그리기
	// DrawDebugBox(GetWorld(), TriggerLocation, BoxHalfExtent, FQuat::Identity, FColor::Green, false, 0.1f, 0, 2.0f);

	if (bOverlap)
	{
		for (const FOverlapResult &Res : OverlapResults)
		{
			AActor *Actor = Res.GetActor();
			if (!Actor)
				continue;

			if (ANetworkPlayer *NP = Cast<ANetworkPlayer>(Actor)) // ANetworkPlayer인지 확인 — 발견 즉시 종료
			{
				return true;
			}
		}
	}
	return false;
}