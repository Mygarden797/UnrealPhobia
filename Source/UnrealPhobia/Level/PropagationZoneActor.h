// Copyright © 2025 Unrealphobia. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PropagationZoneActor.generated.h"


/**
*           Name				: PropagationZoneActor
*           Description		: Manage Area that make sounds propagate
*           Last Update	    : 2025/08/17
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

struct FVoxel
{
    EVoxelType Type = EVoxelType::Empty;
    int32 RoomID = -1;
};

USTRUCT(BlueprintType)
struct FRoomData
{
    GENERATED_BODY()

    UPROPERTY()
    int32 ID = -1;
    UPROPERTY()
    FBox Bounds = FBox(EForceInit::ForceInit);

    UPROPERTY()
    int32 VoxelVolume;

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
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Propagtion Zone")
    TObjectPtr<UBoxComponent> PropagationBounds;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Propagation Zone")
    TSet<AActor*> RegisteredActors;

    UFUNCTION(BlueprintCallable, Category = "Propagation Zone")
    bool ContainsLocation(const FVector& WorldLocation) const;

    const TSet<AActor*>& GetActors() const { return RegisteredActors; };

        
protected:
    UFUNCTION()
    void OnBoundsBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
        const FHitResult& SweepResult);

    UFUNCTION()
    void OnBoundsEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
    UPROPERTY()
    TObjectPtr<USoundPropagationManager> PropagationManager;


public:

    UFUNCTION(BlueprintCallable, Category = "Room Build")
    void GenerateRooms();

    UFUNCTION(BlueprintCallable, Category = "Room Build")
    bool GetRoomAtLocation(const FVector& WorldLocation, FRoomData& OutRoomData) const;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room Build")
    TArray<FRoomData> DiscoveredRooms;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room Setting")
    float VoxelSize = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room Setting")
    TEnumAsByte<ECollisionChannel> WallCollisionChannel = ECC_WorldStatic;

    void DebugDrawRooms(float Duration) const;

private:
    void VoxelizeSpace(const FBox& Room);
    void DiscoverRooms();
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

    static const FIntVector Directions[6];
};
