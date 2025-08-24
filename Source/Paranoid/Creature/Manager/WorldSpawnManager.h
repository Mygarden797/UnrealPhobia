// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "WorldSpawnManager.generated.h"

/**
 * 
 */
UCLASS()
class PARANOID_API UWorldSpawnManager : public UWorldSubsystem
{
	GENERATED_BODY()

	public:
    	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    	virtual void Deinitialize() override;

		static const FString CreatureGrey; //초기 위치 표현
		/*정적 멤버의 특성으로 .h 파일에서 정의할 수 없다.*/
		static const FString CreatureWhiteMask;
		static const FString CreatureZombie;

		class ACreatureBase* SpawnCreature(FVector SpawnPoint, FRotator SpawnRotate,FString Creature);
	
	private:
		//void OnWorldSpawnManagerInitialized(UWorld* World, const UWorld::InitializationValues IVS);
	
};
