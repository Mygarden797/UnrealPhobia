// Copyright © 2025 Unrealphobia. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"
#include "PropagationZoneActor.generated.h"


/**
*           Name				: PropagationZoneActor
*           Description		: Make rooms in zone and manage Actors
*           Last Update	: 2025/09/01, Refactor and Comment
*/

class UBoxComponent;
class USoundPropagationManager;

UENUM(BlueprintType)
enum class EVoxelType : uint8
{
    Empty,
    Wall,
    Door,
    DogHole
};

USTRUCT()
struct FVoxel
{
    GENERATED_BODY()
    
    EVoxelType Type = EVoxelType::Empty;
    int32 RoomID = -1;
};

/* Fundamental Room Data */
USTRUCT(BlueprintType)
struct FRoomData
{
    GENERATED_BODY()

    UPROPERTY()
    int32 ID = -1;
    UPROPERTY()
    FBox Bounds = FBox(EForceInit::ForceInit);

    // 영역 크기
    UPROPERTY()
    int32 VoxelVolume = 0;

    UPROPERTY()
    bool bHasDoor = false;
    UPROPERTY()
    bool bHasDogHole = false;
};

UCLASS()
class UNREALPHOBIA_API APropagationZoneActor : public AActor
{
	GENERATED_BODY()
   
public:

    APropagationZoneActor();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
    /* Define Bounds and Detect Existed Actors */
public:
    // SoundPropagation이 적용될 Zone
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Propagtion Zone")
    TObjectPtr<UBoxComponent> PropagationBounds;

    // Zone 안에 있는 Actors
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Propagation Zone")
    TArray<AActor*> ActorsInZone;

protected:

    // Bound 내의 Actor 등록 및 해제
    UFUNCTION()
    void OnBoundsBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
        const FHitResult& SweepResult);

    UFUNCTION()
    void OnBoundsEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
    // SoundPropagation을 Actor들에 적용
    UPROPERTY()
    TObjectPtr<USoundPropagationManager> PropagationManager;


     /* Define Rooms in Zones */
public:
    virtual void OnConstruction(const FTransform& Transform) override;

    // Separate Room
    UFUNCTION(BlueprintCallable, Category = "Room Build")
    void GenerateRooms();

    UFUNCTION(BlueprintCallable, Category = "Room Build")
    bool GetRoomAtLocation(const FVector& WorldLocation, FRoomData& OutRoomData) const;

    UFUNCTION(CallInEditor, Category = "Room Build")
    void DrawRoomsInEditor();

    UPROPERTY()
    bool bShowDebugBox = false;

    UPROPERTY(EditAnywhere, Category = "Building Size")
    float BoundsMargin = 200.f;

    UPROPERTY(EditAnywhere, Category = "Building Size")
    bool bAudioCalculateBounds = true;

    UPROPERTY(EditAnywhere, Category = "Building Size")
    FName BuildingActorTag = TEXT("Wall");

    // Separated Room Array
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room Build")
    TArray<FRoomData> DiscoveredRooms;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room Build")
    float VoxelSize = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room Setting")
    TEnumAsByte<ECollisionChannel> WallCollisionChannel = ECC_WorldStatic;

    void DebugDrawRooms(float Duration) const;

private:
    void VoxelizeSpace(const FBox& Room);
    void DiscoverRooms();
    void LogRoomDetails() const;
    FRoomData FloodFill(const FIntVector& StartCoord, int32 RoomID);

    int32 ToIndex(const FIntVector& Coord) const;
    int32 ToIndexSafe(const FIntVector& Coord) const;
    FIntVector ToCoord(int32 Index) const;
    bool IsValidCoord(const FIntVector& Coord) const;


    TArray<FVoxel> VoxelGrid;
    FIntVector GridDimensions;
    FVector WorldOrigin;

    TArray<AActor*> DoorActors;
    TArray<AActor*> DogHoleActors;
};
