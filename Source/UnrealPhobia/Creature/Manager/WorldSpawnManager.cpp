// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature/Manager/WorldSpawnManager.h"
#include "Creature/CreatureBase.h"
#include "Creature/CreatureController.h"
#include "Kismet/GameplayStatics.h"


const FString UWorldSpawnManager::CreatureGrey = TEXT("/Game/AI/BP_CreatureGrey.BP_CreatureGrey_C"); //초기 위치 표현
const FString UWorldSpawnManager::CreatureWhiteMask = TEXT("/Game/AI/BP_CreatureWhiteMask.BP_CreatureWhiteMask_C");
const FString UWorldSpawnManager::CreatureZombie = TEXT("/Game/AI/BP_CreatureZombie.BP_CreatureZombie_C");

void UWorldSpawnManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
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
		/* 같은 자리에서 스폰 시에 충돌이 일어나도 스폰해주기 위함이다.*/
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;


	ACreatureBase* SpawnActor = GetWorld()->SpawnActor<ACreatureBase>(BP_Creature,SpawnPoint,SpawnRotate,Params);

	return SpawnActor;
}



