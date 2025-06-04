// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PatrolManager_.generated.h"

/**
 * 
 */
UCLASS()
class UNREALPHOBIA_API UPatrolManager_ : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	public:
    	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    	virtual void Deinitialize() override;

		void RegisterPatrolPoint(AActor* PatrolPoint);
		const TArray<AActor*>& GetPatrolPoints() const;
		int32 GetPatrolPointsNum();
		UPROPERTY()
		TArray<AActor*> PatrolPoints;

	private:
		void OnWorldInitialized(UWorld* World, const UWorld::InitializationValues IVS);
	
};
