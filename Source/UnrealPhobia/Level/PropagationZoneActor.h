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

UCLASS()
class UNREALPHOBIA_API APropagationZoneActor : public AActor
{
	GENERATED_BODY()
	
public:
	APropagationZoneActor();

	virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

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
};
