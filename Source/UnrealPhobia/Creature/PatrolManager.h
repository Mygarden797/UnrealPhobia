// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolManager.generated.h"

UCLASS()
class UNREALPHOBIA_API APatrolManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APatrolManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditInstanceOnly,BlueprintReadWrite,Category="AI")
	TArray<AActor*> PatrolPoints;

	static APatrolManager* GetInstance(UWorld* WorldContext);

	int32 GetPatrolPointsNum();

private:
	static APatrolManager* Instance;

};
