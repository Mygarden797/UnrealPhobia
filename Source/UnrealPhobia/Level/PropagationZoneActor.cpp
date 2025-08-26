// Copyright © 2025 Unrealphobia. All Rights Reserved.


#include "Level/PropagationZoneActor.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "EngineUtils.h"
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

    GenerateRooms();
    UE_LOG(LogTemp, Warning, TEXT("PropagationZoneActor::BeginPlay(): Found %d rooms."), DiscoveredRooms.Num());
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

const FIntVector APropagationZoneActor::Directions[6] = {
    {1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}
};

void APropagationZoneActor::GenerateRooms()
{
    UBoxComponent* BoundsComponent = GetOwner()->FindComponentByClass<UBoxComponent>();
    if (!BoundsComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("No UBoxComponent found on owner."));
        return;
    }

    DiscoveredRooms.Empty();
    VoxelGrid.Empty();

    const FBox AnalysisBox = BoundsComponent->CalcBounds(BoundsComponent->GetComponentTransform()).GetBox();
    const FVector AnalysisSize = AnalysisBox.GetSize();
    WorldOrigin = AnalysisBox.Min;

    GridDimensions.X = FMath::CeilToInt(AnalysisSize.X / VoxelSize);
    GridDimensions.Y = FMath::CeilToInt(AnalysisSize.Y / VoxelSize);
    GridDimensions.Z = FMath::CeilToInt(AnalysisSize.Z / VoxelSize);

    if (GridDimensions.X <= 0 || GridDimensions.Y <= 0 || GridDimensions.Z <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid grid dimensions."));
        return;
    }

    // 문/개구멍 액터 미리 탐지
    DoorActors.Empty();
    DogHoleActors.Empty();
    for (TActorIterator<AActor> It(GetWorld()); It; ++It)
    {
        AActor* Actor = *It;
        if (Actor->ActorHasTag("Wall_Door")) DoorActors.Add(Actor);
        if (Actor->ActorHasTag("Wall_Hole")) DogHoleActors.Add(Actor);
    }

    VoxelizeSpace(AnalysisBox);
    DiscoverRooms();

    UE_LOG(LogTemp, Warning, TEXT("Room analysis complete. Found %d rooms."), DiscoveredRooms.Num());
}

void APropagationZoneActor::VoxelizeSpace(const FBox& AnalysisBox)
{
    VoxelGrid.Init(FVoxel(), GridDimensions.X * GridDimensions.Y * GridDimensions.Z);
    UWorld* World = GetWorld();
    const FVector HalfVoxel = FVector(VoxelSize / 2.0f);

    for (int32 Index = 0; Index < VoxelGrid.Num(); ++Index)
    {
        const FIntVector Coord = ToCoord(Index);
        const FVector VoxelWorldPos = WorldOrigin + FVector(Coord) * VoxelSize + HalfVoxel;

        FCollisionQueryParams Params;
        Params.AddIgnoredActor(GetOwner());
        if (World->OverlapBlockingTestByChannel(VoxelWorldPos, FQuat::Identity, WallCollisionChannel, FCollisionShape::MakeBox(HalfVoxel), Params))
        {
            VoxelGrid[Index].Type = EVoxelType::Wall;
        }
        else
        {
            VoxelGrid[Index].Type = EVoxelType::Empty;

            for (AActor* Door : DoorActors)
            {
                if (Door->GetComponentsBoundingBox().IsInside(VoxelWorldPos))
                {
                    VoxelGrid[Index].Type = EVoxelType::Door;
                    break;
                }
            }
            if (VoxelGrid[Index].Type == EVoxelType::Empty)
            {
                for (AActor* Crawl : DogHoleActors)
                {
                    if (Crawl->GetComponentsBoundingBox().IsInside(VoxelWorldPos))
                    {
                        VoxelGrid[Index].Type = EVoxelType::DogHole;
                        break;
                    }
                }
            }
        }
    }
}

void APropagationZoneActor::DiscoverRooms()
{
    int32 CurrentRoomID = 1;
    for (int32 Index = 0; Index < VoxelGrid.Num(); ++Index)
    {
        if (VoxelGrid[Index].Type == EVoxelType::Empty && VoxelGrid[Index].RoomID == -1)
        {
            FRoomData NewRoom = FloodFill(ToCoord(Index), CurrentRoomID);
            DiscoveredRooms.Add(NewRoom);
            CurrentRoomID++;
        }
    }
}

FRoomData APropagationZoneActor::FloodFill(const FIntVector& StartCoord, int32 RoomID)
{
    FRoomData RoomData;
    RoomData.ID = RoomID;

    TQueue<FIntVector> Queue;
    Queue.Enqueue(StartCoord);
    VoxelGrid[ToIndex(StartCoord)].RoomID = RoomID;

    FVector Min = FVector(FLT_MAX);
    FVector Max = FVector(-FLT_MAX);
    int32 VoxelCounter = 0;

    while (!Queue.IsEmpty())
    {
        FIntVector Current;
        Queue.Dequeue(Current);
        int32 Index = ToIndex(Current);
        VoxelCounter++;

        FVector VoxelMin = WorldOrigin + FVector(Current) * VoxelSize;
        Min.X = FMath::Min(Min.X, VoxelMin.X);
        Min.Y = FMath::Min(Min.Y, VoxelMin.Y);
        Min.Z = FMath::Min(Min.Z, VoxelMin.Z);
        Max.X = FMath::Max(Max.X, VoxelMin.X + VoxelSize);
        Max.Y = FMath::Max(Max.Y, VoxelMin.Y + VoxelSize);
        Max.Z = FMath::Max(Max.Z, VoxelMin.Z + VoxelSize);

        for (const auto& Dir : Directions)
        {
            FIntVector Neighbor = Current + Dir;
            if (!IsValidCoord(Neighbor)) continue;

            FVoxel& NVoxel = VoxelGrid[ToIndex(Neighbor)];
            if (NVoxel.RoomID != -1) continue;

            if (NVoxel.Type == EVoxelType::Empty)
            {
                NVoxel.RoomID = RoomID;
                Queue.Enqueue(Neighbor);
            }
            else if (NVoxel.Type == EVoxelType::Door)
            {
                RoomData.bHasDoor = true;
            }
            else if (NVoxel.Type == EVoxelType::DogHole)
            {
                RoomData.bHasDogHole = true;
            }
        }
    }

    RoomData.Bounds = FBox(Min, Max);
    RoomData.VoxelVolume = VoxelCounter;
    return RoomData;
}

bool APropagationZoneActor::GetRoomAtLocation(const FVector& WorldLocation, FRoomData& OutRoomData) const
{
    if (GridDimensions.IsZero()) return false;

    FVector Local = WorldLocation - WorldOrigin;
    FIntVector Coord(
        FMath::FloorToInt(Local.X / VoxelSize),
        FMath::FloorToInt(Local.Y / VoxelSize),
        FMath::FloorToInt(Local.Z / VoxelSize)
    );

    if (!IsValidCoord(Coord)) return false;
    int32 Index = ToIndex(Coord);
    int32 RoomID = VoxelGrid[Index].RoomID;
    if (RoomID > 0)
    {
        OutRoomData = DiscoveredRooms[RoomID - 1];
        return true;
    }
    return false;
}

int32 APropagationZoneActor::ToIndex(const FIntVector& Coord) const
{
    return (Coord.Z * GridDimensions.X * GridDimensions.Y) + (Coord.Y * GridDimensions.X) + Coord.X;
}

int32 APropagationZoneActor::ToIndexSafe(const FIntVector& Coord) const
{
    return IsValidCoord(Coord) ? ToIndex(Coord) : INDEX_NONE;
}

FIntVector APropagationZoneActor::ToCoord(int32 Index) const
{
    int32 Z = Index / (GridDimensions.X * GridDimensions.Y);
    int32 Y = (Index / GridDimensions.X) % GridDimensions.Y;
    int32 X = Index % GridDimensions.X;
    return FIntVector(X, Y, Z);
}

bool APropagationZoneActor::IsValidCoord(const FIntVector& Coord) const
{
    return Coord.X >= 0 && Coord.X < GridDimensions.X &&
        Coord.Y >= 0 && Coord.Y < GridDimensions.Y &&
        Coord.Z >= 0 && Coord.Z < GridDimensions.Z;
}

void APropagationZoneActor::DebugDrawRooms(float Duration) const
{
    for (const FRoomData& Room : DiscoveredRooms)
    {
        DrawDebugBox(GetWorld(), Room.Bounds.GetCenter(), Room.Bounds.GetExtent(), FColor::Green, false, Duration, 0, 2.0f);
    }
}