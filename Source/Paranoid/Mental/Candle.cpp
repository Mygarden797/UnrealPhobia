#include "Mental/Candle.h"
#include "Components/StaticMeshComponent.h"

ACandle::ACandle()
{
    PrimaryActorTick.bCanEverTick = false;

    CandleMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CandleMesh"));
    RootComponent = CandleMeshComponent;
}

void ACandle::BeginPlay()
{
    Super::BeginPlay();

    if (!FlameOnMaterial)
    {
        FlameOnMaterial = CandleMeshComponent->GetMaterial(FlameMaterialSlotIndex);
    }
    
    SetFlameActive(false);
}

void ACandle::SetFlameActive(bool bIsActive)
{
    if (bIsActive)
    {
        if (FlameOnMaterial)
        {
            CandleMeshComponent->SetMaterial(FlameMaterialSlotIndex, FlameOnMaterial);
        }
    }
    else
    {
        if (FlameOffMaterial)
        {
            CandleMeshComponent->SetMaterial(FlameMaterialSlotIndex, FlameOffMaterial);
        }
    }
}
