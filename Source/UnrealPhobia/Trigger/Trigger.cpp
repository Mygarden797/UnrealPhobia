#include "Trigger.h"
#include "Components/StaticMeshComponent.h"

ATrigger::ATrigger()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create components
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = BaseMesh;
}

void ATrigger::OnConstruction(const FTransform &Transform)
{
	Super::OnConstruction(Transform);

	// Assign mesh based on enum
	switch (TriggerName)
	{
	case ETriggerName::Grey:
		if (GreyTriggerMesh)
		{
			BaseMesh->SetStaticMesh(GreyTriggerMesh);
			BaseMesh->SetRenderCustomDepth(true);
			BaseMesh->CustomDepthStencilValue = 1; // Stencil 값이 필요할 때만
		}
		break;

	case ETriggerName::Ghost:
		if (GhostTriggerMesh)
		{
			BaseMesh->SetStaticMesh(GhostTriggerMesh);
		}
		break;

	default:
		break;
	}
}

void ATrigger::BeginPlay()
{
	Super::BeginPlay();
}

void ATrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}