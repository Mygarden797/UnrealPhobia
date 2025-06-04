// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "WorldSpawnManager.generated.h"

/**
 * 
 */
UCLASS()
class UNREALPHOBIA_API UWorldSpawnManager : public UWorldSubsystem
{
	GENERATED_BODY()

	public:
    	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    	virtual void Deinitialize() override;

		const FString CreatureGrey = TEXT("/Game/AI/BP_CreatureGrey.BP_CreatureGrey_C"); //초기 위치 표현

		class ACreatureBase* SpawnCreature(FVector SpawnPoint, FRotator SpawnRotate,FString Creature);
	
	private:
		void OnWorldSpawnManagerInitialized(UWorld* World, const UWorld::InitializationValues IVS);
	
};
