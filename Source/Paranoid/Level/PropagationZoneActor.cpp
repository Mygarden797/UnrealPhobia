// Copyright © 2025 Unrealphobia. All Rights Reserved.


#include "Level/PropagationZoneActor.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "Managers/SoundPropagationManager.h"

APropagationZoneActor::APropagationZoneActor()
{
	PrimaryActorTick.bCanEverTick = false;

    PropagationBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("Propagation Bounds"));
    SetRootComponent(PropagationBounds);

    PropagationBounds->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    PropagationBounds->SetCollisionObjectType(ECC_WorldStatic);
    PropagationBounds->SetCollisionResponseToAllChannels(ECR_Ignore);
    PropagationBounds->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    PropagationBounds->SetGenerateOverlapEvents(true);

}

void APropagationZoneActor::BeginPlay()
{
	Super::BeginPlay();

    PropagationBounds->OnComponentBeginOverlap.AddDynamic(this, &APropagationZoneActor::OnBoundsBeginOverlap);
    PropagationBounds->OnComponentEndOverlap.AddDynamic(this, &APropagationZoneActor::OnBoundsEndOverlap);


    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* GI = World->GetGameInstance())
        {
            PropagationManager = GI->GetSubsystem<USoundPropagationManager>();
            if (IsValid(PropagationManager))
            {
                PropagationManager->RegisterZone(this);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("PropagationZoneActor::BeginPlay(): No PropagationManager"));
                return;
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("PropagationZoneActor::BeginPlay(): No GI"));
            return;
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PropagationZoneActor::BeginPlay(): No World"));
        return;
    }
}

void APropagationZoneActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (PropagationManager)
    {
        PropagationManager->UnregisterZone(this);
    }
    Super::EndPlay(EndPlayReason);
}

bool APropagationZoneActor::ContainsLocation(const FVector& WorldLocation) const
{
    FVector Origin, Extent;
    const FBox Box(Origin - Extent, Origin + Extent);
    return Box.IsInside(WorldLocation);
}

void APropagationZoneActor::OnBoundsBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == this) return;

    if (!OtherActor->IsA<ACharacter>()) return;

    RegisteredActors.Add(OtherActor);

    if (IsValid(PropagationManager))
    {
        PropagationManager->NotifyActorEntered(this, OtherActor);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("APropagationZoneActor::OnBoundsBeginOverlap(): No PropagationManager"));
        return;
    }
}

void APropagationZoneActor::OnBoundsEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (!OtherActor) return;

    if (RegisteredActors.Contains(OtherActor))
    {
        RegisteredActors.Remove(OtherActor);
        if (IsValid(PropagationManager))
        {
            PropagationManager->NotifyActorExited(this, OtherActor);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("APropagationZoneActor::OnBoundsEndOverlap(): No PropagationManager"));
            return;
        }
    }
}
