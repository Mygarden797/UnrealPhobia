// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Trigger/Trigger.h"
#include "Network/NetworkBase.h"
#include "NetworkMirror.generated.h"

UCLASS()
class UNREALPHOBIA_API ANetworkMirror : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANetworkMirror();


    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StaticMesh")
    UStaticMeshComponent* BaseMeshComponent; // Default mesh component

    UFUNCTION(BlueprintCallable)
    bool ActivateTrigger(int64 trigger_id);

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName location_id;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    void SetupMirrorMesh();




public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;
};
