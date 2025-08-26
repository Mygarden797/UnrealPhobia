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
    bool hHasDogHole = false;
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


    /* Separate Room with Flood Fill Algorithm */
    void ComputeRooms();
    void FloodFillRoom(TArray<TArray<TArray<int32>>>& Grid,
        int32 StartX, int32 StartY, int32 StartZ,
        int32 RoomId,
        FRoomInfo& OutRoom,
        const FVector& Origin);

    bool CheckDoorInRoom(const FRoomInfo& Room);
    bool CheckDogHoleInRoom(const FRoomInfo& Room);
        
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

    UPROPERTY()
    TArray<FRoomInfo> Rooms;

    UPROPERTY()
    TArray<AActor*> DoorActors;
    TArray<AActor*> DogHoleActors;

    float CellSize = 100.f;
};
