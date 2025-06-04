// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature/Manager/WorldSpawnManager.h"
#include "Creature/CreatureBase.h"
#include "Creature/CreatureController.h"
#include "Kismet/GameplayStatics.h"


void UWorldSpawnManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &UWorldSpawnManager::OnWorldSpawnManagerInitialized);
}

void UWorldSpawnManager::Deinitialize()
{
    Super::Deinitialize();
}

/*Creature를 Spawn하기 위한 SpawnActor의 Wrapper 함수*/
class ACreatureBase* UWorldSpawnManager::SpawnCreature(FVector SpawnPoint, FRotator SpawnRotate,FString Creature)
{
    TSubclassOf<ACreatureBase> BP_Creature;
	BP_Creature = LoadClass<ACreatureBase>(nullptr,*Creature);
	if(BP_Creature == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("BP_Creature is not Load"));
		return nullptr;
	}
	FActorSpawnParameters Params;

	ACreatureBase* SpawnActor = GetWorld()->SpawnActor<ACreatureBase>(BP_Creature,SpawnPoint,SpawnRotate,Params);

	return SpawnActor;
}

void UWorldSpawnManager::OnWorldSpawnManagerInitialized(UWorld* World, const UWorld::InitializationValues IVS)
{
    /*시작 시 실험을 위해서 캐릭터를 스폰시키는 코드*/
    SpawnCreature(FVector::ZeroVector,FRotator::ZeroRotator,CreatureGrey);
	// SpawnCreature(FVector::ZeroVector,FRotator::ZeroRotator,CreatureGrey);
}

