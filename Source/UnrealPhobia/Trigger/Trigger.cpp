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
	SetupTriggerMaterial();
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
		break;
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
}

void ATrigger::SetupTriggerMaterial()
{
	FString MaterialPath = TEXT("/Script/Engine.Material'/Game/Trigger/Materials/M_Highlight.M_Highlight'"), EmptyMaterialPath = TEXT("/Script/Engine.Material'/Game/Trigger/Materials/M_Empty.M_Empty'");

	HighlightMaterial = Cast<UMaterialInterface>(StaticLoadObject(
		UMaterialInterface::StaticClass(), // 로드할 객체의 클래스 타입 지정 (UMaterialInterface)
		nullptr,						   // 패키지 지정
		*MaterialPath					   // 로드할 에셋의 경로
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

	EmptyMaterial = Cast<UMaterialInterface>(StaticLoadObject(
		UMaterialInterface::StaticClass(), // 로드할 객체의 클래스 타입 지정 (UMaterialInterface)
		nullptr,						   // 패키지 지정
		*EmptyMaterialPath				   // 로드할 에셋의 경로
		));

	if (EmptyMaterial)
	{
		UE_LOG(LogTemp, Log, TEXT("Loaded material from %s"), // 머티리얼 경로를 포함한 로드 성공 로그 출력,
			   *EmptyMaterialPath);
	}
	else
	{
		// 에러 로그 출력
		UE_LOG(LogTemp, Error, TEXT("Failed to load material at %s"), *EmptyMaterialPath);
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

	if (!IsValid(BaseMeshComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("BaseMeshComponent is null or invalid on %s"), *GetNameSafe(this));
		return;
	}

	if (!HighlightMaterial || !EmptyMaterial)
	{
		UE_LOG(LogTemp, Error, TEXT("Trigger Overlay Material is nullptr"));
		return;
	}

	if (!IsValid(HighlightMaterial) || !IsValid(EmptyMaterial) || !HighlightMaterial->IsValidLowLevel() || !EmptyMaterial->IsValidLowLevel())
	{
		SetupTriggerMaterial();
		UE_LOG(LogTemp, Warning, TEXT("Trigger Material is invalid,. Resetting.."));
		return;
	}

	bool bShouldHighlight = false;
	bShouldHighlight = CheckHighlight();

	if (bShouldHighlight)
	{
		if (!bIsHighlighting)
		{
			UE_LOG(LogTemp, Log, TEXT("Turning highlight ON for %s"), *GetNameSafe(this));
			if (HighlightMaterial && HighlightMaterial->IsValidLowLevel())
			{
				BaseMeshComponent->SetOverlayMaterial(HighlightMaterial);
			}
			bIsHighlighting = true;
		}
	}
	else
	{
		if (bIsHighlighting)
		{
			UE_LOG(LogTemp, Log, TEXT("Turning highlight OFF for %s"), *GetNameSafe(this));
			if (EmptyMaterial && EmptyMaterial->IsValidLowLevel())
			{
				BaseMeshComponent->SetOverlayMaterial(EmptyMaterial);
				SetupTriggerMesh();
			}
			bIsHighlighting = false;
		}
	}
}

bool ATrigger::CheckHighlight()
{
	UWorld *World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("CheckHighlight: GetWorld() == nullptr on %s"), *GetNameSafe(this));
		return false;
	}

	TriggerLocation = GetActorLocation();

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

	FVector BoxHalfExtent(230.0f, 230.0f, 80.0f);
	FCollisionShape CollisionShape = FCollisionShape::MakeBox(BoxHalfExtent);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	TArray<FOverlapResult> OverlapResults;
	bool bOverlap = World->OverlapMultiByObjectType(
		OverlapResults,
		TriggerLocation,
		FQuat::Identity,
		ObjectQueryParams,
		CollisionShape,
		QueryParams);

	// DrawDebugBox(World, TriggerLocation, BoxHalfExtent, FQuat::Identity, FColor::Green, false, 0.1f, 0, 2.0f);

	if (bOverlap)
	{
		for (const FOverlapResult &Res : OverlapResults)
		{
			AActor *Actor = Res.GetActor();
			if (!IsValid(Actor)) // null 또는 PendingKill 검사
				continue;

			if (ANetworkPlayer *NP = Cast<ANetworkPlayer>(Actor))
			{
				return true;
			}
		}
	}
	return false;
}