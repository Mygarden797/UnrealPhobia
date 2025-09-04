// Copyright © 2025 Unrealphobia. All Rights Reserved.


#include "Level/PropagationZoneActor.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"
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

    PropagationBounds->OnComponentBeginOverlap.AddDynamic(this, &APropagationZoneActor::OnBoundsBeginOverlap);
    PropagationBounds->OnComponentEndOverlap.AddDynamic(this, &APropagationZoneActor::OnBoundsEndOverlap);
}

void APropagationZoneActor::BeginPlay()
{
    Super::BeginPlay();

    // 처음부터 존재하는 Actor 등록
    TArray<AActor*> OverlappingActors;
    PropagationBounds->GetOverlappingActors(OverlappingActors, ACharacter::StaticClass());

    UE_LOG(LogTemp, Warning, TEXT("APropagationZoneActor::BeingPlay(): Found %d overlapping actors"), OverlappingActors.Num());
    for (AActor* Actor : OverlappingActors)
    {
        if (IsValid(Actor))
        {
            if (Actor->IsA(APropagationZoneActor::StaticClass()))
            {
                // UE_LOG(LogTemp, Warning, TEXT("APropagationZoneActor::BeginPlay(): PrpagationZoneActor is excepted"));
                continue;
            }
            ActorsInZone.Add(Actor);
            UE_LOG(LogTemp, Display, TEXT("APropagationZoneActor::BeingPlay(): Add an Actor %s in ActorToZone"), *GetNameSafe(Actor));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("APropagationZoneActor::BeginPlay(): Invalid Actor in Zone"));
        }

#if WITH_EDITOR
        if (GIsEditor)  // 에디터에서만 실행
        {
            UE_LOG(LogTemp, Error, TEXT("=== Editor Mode Detected ==="));
            DrawRoomsInEditor();  // 직접 호출
        }
#endif
    }

    /*
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
    */

    GenerateRooms();
}

void APropagationZoneActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Zone에 속한 Actor, Room 정리
    ActorsInZone.Empty();
    DiscoveredRooms.Empty();
    UE_LOG(LogTemp, Warning, TEXT("APropagationZoneActor::EndPlay(): Clear ActorsInZone and DiscoveredRooms"));
    Super::EndPlay(EndPlayReason);
}

void APropagationZoneActor::OnBoundsBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)

    /**
    * OverlappedComp : 겹쳐진, 충돌한 컴포넌트
    * OtherActor : 충돌한 액터
    * OtherComp : 충돌한 액터의 컴포넌트
    * OtherBodyIndex : Physics Body Index
    */
{
    if (!OtherActor || OtherActor == this) return;


    // Character로 된 Survivor, NetworkPlayer, CreatureBase가 아니면 리턴
    if (!(OtherActor->IsA<ACharacter>())) return;

    ActorsInZone.Add(OtherActor);
    UE_LOG(LogTemp, Log, TEXT("APropagationZoneActor::OnBoundsBeginOverlap() : Actor %s is added"), *GetNameSafe(OtherActor))
}

void APropagationZoneActor::OnBoundsEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (!OtherActor) return;

    if (ActorsInZone.Contains(OtherActor))
    {
        ActorsInZone.Remove(OtherActor);
        UE_LOG(LogTemp, Log, TEXT("APropagationZoneActor::OnBoundsEndOverlap() : Actor % s is removed"), *GetNameSafe(OtherActor))
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("APropagationZoneActor::OnBoundsEndOverlap(): Actor %s is not contained"), *GetNameSafe(OtherActor))
    }
}

void APropagationZoneActor::GenerateRooms()
{
    // PropagationBounds = GetOwner()->FindComponentByClass<UBoxComponent>();
    if (!PropagationBounds)
    {
        UE_LOG(LogTemp, Error, TEXT("APropagationZoneActor::GenerateRooms(): No UBoxComponent found on owner."));
        return;
    }

    DiscoveredRooms.Empty();
    VoxelGrid.Empty();

    // PropagationBounds의 영역 크기 계산
    const FBox AnalysisBox = PropagationBounds->CalcBounds(PropagationBounds->GetComponentTransform()).GetBox();
    const FVector AnalysisSize = AnalysisBox.GetSize();

    // Bounds의 최소 좌표 설정
    // 예: (500, -200, 100), 이제 Grid 좌표에서는 (0, 0, 0)으로 변환됨
    WorldOrigin = AnalysisBox.Min;

    // 영역 크기를 VoxelSize로 나누고 올림
    GridDimensions.X = FMath::CeilToInt(AnalysisSize.X / VoxelSize);
    GridDimensions.Y = FMath::CeilToInt(AnalysisSize.Y / VoxelSize);
    GridDimensions.Z = FMath::CeilToInt(AnalysisSize.Z / VoxelSize);

    if (GridDimensions.X <= 0 || GridDimensions.Y <= 0 || GridDimensions.Z <= 0)
    {
        UE_LOG(LogTemp, Error, TEXT("APropagationZoneActor::GenerateRooms(): Invalid grid dimensions."));
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

    UE_LOG(LogTemp, Warning, TEXT("APropagationZoneActor::GenerateRooms(): %s generates Rooms. Found %d rooms."), *(GetNameSafe(this)), DiscoveredRooms.Num());
}

void APropagationZoneActor::VoxelizeSpace(const FBox& AnalysisBox)
{
    // Initalize Voxel Array, 전체 X*Y*Z개의 Voxel 생성
    VoxelGrid.Init(FVoxel(), GridDimensions.X * GridDimensions.Y * GridDimensions.Z);
    UWorld* World = GetWorld();

    // Length between Center and Edge
    const FVector HalfVoxel = FVector(VoxelSize / 2.0f);

    for (int32 Index = 0; Index < VoxelGrid.Num(); ++Index)
    {
        // 1차원 Index를 3차원 Grid 좌표로 변환
        const FIntVector Coord = ToCoord(Index);
        // 그리드 좌표를 월드 좌표로 변환
        const FVector VoxelWorldPos = WorldOrigin + FVector(Coord) * VoxelSize + HalfVoxel;

        // Collision Parameter
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(GetOwner());

        // Voxel Center부터 충돌 검사
        if (World->OverlapBlockingTestByChannel(VoxelWorldPos, FQuat::Identity, WallCollisionChannel, FCollisionShape::MakeBox(HalfVoxel), Params))
        {
            VoxelGrid[Index].Type = EVoxelType::Wall;
        }
        else
        {
            VoxelGrid[Index].Type = EVoxelType::Empty;

            // 문인 경우
            for (AActor* Door : DoorActors)
            {
                if (Door->GetComponentsBoundingBox().IsInside(VoxelWorldPos))
                {
                    VoxelGrid[Index].Type = EVoxelType::Door;
                    break;
                }
            }

            // 문도 아니면 개구멍
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
        // 빈 공간에 대해 FloodFill 실행, 방의 모든 Voxel에 같은 ID 부여
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
    // BFS 알고리즘 기반 

    // X, Y, Z 방향 벡터
    const FIntVector Directions[6] = { {1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1} };

    FRoomData RoomData;
    RoomData.ID = RoomID;

    TQueue<FIntVector> Queue;
    Queue.Enqueue(StartCoord);
    VoxelGrid[ToIndex(StartCoord)].RoomID = RoomID;

    // float 최대 및 최소
    FVector Min = FVector(FLT_MAX);
    FVector Max = FVector(-FLT_MAX);
    int32 VoxelCounter = 0;

    while (!Queue.IsEmpty())
    {
        FIntVector Current;
        Queue.Dequeue(Current);
        int32 Index = ToIndex(Current);
        VoxelCounter++;

        // Voxel 최소 좌표 설정
        FVector VoxelMin = WorldOrigin + FVector(Current) * VoxelSize;
        Min.X = FMath::Min(Min.X, VoxelMin.X);
        Min.Y = FMath::Min(Min.Y, VoxelMin.Y);
        Min.Z = FMath::Min(Min.Z, VoxelMin.Z);
        Max.X = FMath::Max(Max.X, VoxelMin.X + VoxelSize);
        Max.Y = FMath::Max(Max.Y, VoxelMin.Y + VoxelSize);
        Max.Z = FMath::Max(Max.Z, VoxelMin.Z + VoxelSize);

        // 모든 방향에 대한 Neighbor Search
        for (const auto& Dir : Directions)
        {
            // 현재 좌표에서 방향 벡터 더함
            FIntVector Neighbor = Current + Dir;
            if (!IsValidCoord(Neighbor))
            {
                //vUE_LOG(LogTemp, Warning, TEXT("APropagationZoneActor::FloodFill(): Array is out of range"));
                continue;
            }

            FVoxel& NVoxel = VoxelGrid[ToIndex(Neighbor)];
            if (NVoxel.RoomID != -1)
            {
                // UE_LOG(LogTemp, Warning, TEXT("APropagationZoneActor::FloodFill(): The Voxel is already allocated")); 
                continue;
            }

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
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("No valid world for debug drawing"));
        return;
    }

    // 기존 디버그 라인 제거
    FlushPersistentDebugLines(World);

    // UE_LOG(LogTemp, Warning, TEXT("Drawing %d rooms"), DiscoveredRooms.Num());

    for (int32 i = 0; i < DiscoveredRooms.Num(); ++i)
    {
        const FRoomData& Room = DiscoveredRooms[i];

        // 다양한 색상으로 구분
        FColor RoomColor = FColor::MakeRedToGreenColorFromScalar(
            float(i) / FMath::Max(1, DiscoveredRooms.Num() - 1));

        // 로그로 각 방 정보 확인
        UE_LOG(LogTemp, Log, TEXT("Room %d: Center=(%s), Extent=(%s)"),
            Room.ID,
            *Room.Bounds.GetCenter().ToString(),
            *Room.Bounds.GetExtent().ToString());

        // DrawDebugBox 호출
        DrawDebugBox(World,
            Room.Bounds.GetCenter(),
            Room.Bounds.GetExtent(),
            RoomColor,
            true,        // bPersistentLines
            Duration,    // LifeTime (-1 = 영구)
            0,           // DepthPriority  
            5.0f);       // Thickness (더 두껍게)

        // 추가로 DrawDebugString도 시도
        DrawDebugString(World,
            Room.Bounds.GetCenter(),
            FString::Printf(TEXT("Room %d"), Room.ID),
            nullptr,
            FColor::White,
            Duration,
            true,        // bDrawShadow
            5.0f);       // FontScale
    }
}

void APropagationZoneActor::DrawRoomsInEditor()
{
    if (bShowDebugBox)
    {
        FlushPersistentDebugLines(GetWorld());
        FlushDebugStrings(GetWorld());
        bShowDebugBox = false;
        UE_LOG(LogTemp, Log, TEXT("APropagationZoneActor::DrawRoomsInEditor(): Deactivate Debug Box"));
    }
    else
    {
        DebugDrawRooms(-1.0f);
        LogRoomDetails();
        bShowDebugBox = true;
        UE_LOG(LogTemp, Log, TEXT("APropagationZoneActor::DrawRoomsInEditor(): Activate Debug Box"));
    }
}

void APropagationZoneActor::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    GenerateRooms();
    DrawRoomsInEditor();
}

void APropagationZoneActor::LogRoomDetails() const
{
    UE_LOG(LogTemp, Log, TEXT("APropagationZoneActor::LogRoomDetails"));
    UE_LOG(LogTemp, Log, TEXT("---- Total Rooms ----"));

    UE_LOG(LogTemp, Warning, TEXT("=== Detailed Room Information ==="));

    int32 RoomsWithDoors = 0;
    int32 RoomsWithHoles = 0;
    int32 TotalVoxels = 0;

    for (const FRoomData& Room : DiscoveredRooms)
    {
        // 각 방 정보
        UE_LOG(LogTemp, Warning, TEXT("Room %d:"), Room.ID);
        UE_LOG(LogTemp, Warning, TEXT("  - Has Door: %s"), Room.bHasDoor ? TEXT("YES") : TEXT("NO"));
        UE_LOG(LogTemp, Warning, TEXT("  - Has Hole: %s"), Room.bHasDogHole ? TEXT("YES") : TEXT("NO"));
        UE_LOG(LogTemp, Warning, TEXT("  - Volume: %d voxels (%.2f m³)"),
            Room.VoxelVolume, (Room.VoxelVolume * FMath::Pow(VoxelSize / 100.0f, 3)));
        UE_LOG(LogTemp, Warning, TEXT("  - Center: %s"), *Room.Bounds.GetCenter().ToString());
        UE_LOG(LogTemp, Warning, TEXT("  - Size: %.1f x %.1f x %.1f cm"),
            Room.Bounds.GetSize().X, Room.Bounds.GetSize().Y, Room.Bounds.GetSize().Z);
        UE_LOG(LogTemp, Warning, TEXT("  ---"));

        // 통계 수집
        if (Room.bHasDoor) RoomsWithDoors++;
        if (Room.bHasDogHole) RoomsWithHoles++;
        TotalVoxels += Room.VoxelVolume;
    }

    // 요약 통계
    UE_LOG(LogTemp, Warning, TEXT("=== Summary ==="));
    UE_LOG(LogTemp, Warning, TEXT("Rooms with Doors: %d/%d"), RoomsWithDoors, DiscoveredRooms.Num());
    UE_LOG(LogTemp, Warning, TEXT("Rooms with Holes: %d/%d"), RoomsWithHoles, DiscoveredRooms.Num());
    UE_LOG(LogTemp, Warning, TEXT("Total Voxels: %d"), TotalVoxels);
    UE_LOG(LogTemp, Warning, TEXT("Average Room Size: %.1f voxels"),
        DiscoveredRooms.Num() > 0 ? float(TotalVoxels) / DiscoveredRooms.Num() : 0.0f);
}