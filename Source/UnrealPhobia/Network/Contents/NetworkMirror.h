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
    //임시용
    ~ANetworkMirror()
    {
        delete TriggerInfo;
        delete CreatureInfo;
        TriggerInfo = nullptr;
        CreatureInfo = nullptr;
    }

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StaticMesh")
    UStaticMeshComponent* BaseMeshComponent; // Default mesh component

    UFUNCTION(BlueprintCallable)
    bool ActivateTrigger();
    bool ActivateTrigger(int64 trigger_id);

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    void SetupMirrorMesh();


    //임시 트리거 정보
    class Protocol::ObjectInfo* TriggerInfo; 
    class Protocol::ObjectInfo* CreatureInfo;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;
};
