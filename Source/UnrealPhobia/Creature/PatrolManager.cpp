// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature/PatrolManager.h"
#include "Kismet/GameplayStatics.h"
#include "Creature/Manager/WorldSpawnManager.h"

// Sets default values
APatrolManager::APatrolManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void APatrolManager::BeginPlay()
{
	Super::BeginPlay();
	UWorldSpawnManager* SubSystem = GetWorld()->GetSubsystem<UWorldSpawnManager>();
	if(SubSystem == nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("SubSystem is Null"));
		return;
	}

	SubSystem->SpawnCreature(FVector::ZeroVector,FRotator::ZeroRotator,UWorldSpawnManager::CreatureGrey);
	FVector SpawnPoint = FVector(10,10,10);
	FVector SpawnPoint2 = FVector(100,100,100);
	FVector SpawnPoint3 = FVector(50,50,50);
	SubSystem->SpawnCreature(SpawnPoint,FRotator::ZeroRotator,UWorldSpawnManager::CreatureGrey);
	SubSystem->SpawnCreature(SpawnPoint2,FRotator::ZeroRotator,UWorldSpawnManager::CreatureWhiteMask);
	SubSystem->SpawnCreature(SpawnPoint3,FRotator::ZeroRotator,UWorldSpawnManager::CreatureZombie);
	
}

// Called every frame
void APatrolManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


