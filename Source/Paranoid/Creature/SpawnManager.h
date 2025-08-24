// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnManager.generated.h"

UCLASS()
class PARANOID_API ASpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnManager();
	const FString CreatureGrey = TEXT("/Game/AI/BP_CreatureGrey.BP_CreatureGrey_C"); //초기 위치 표현

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	static ASpawnManager* GetInstance(UWorld* WorldContext);
	class ACreatureBase* SpawnCreature(FVector SpawnPoint, FRotator SpawnRotate,FString Creature);


private:
	static ASpawnManager* Instance;
	int32 SpawnCreatureNum = 0;
	

};
