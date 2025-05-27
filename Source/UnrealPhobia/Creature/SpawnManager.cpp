// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature/SpawnManager.h"
#include "Creature/CreatureBase.h"
#include "Creature/CreatureController.h"
#include "Kismet/GameplayStatics.h"

ASpawnManager* ASpawnManager::Instance = nullptr;


// Sets default values
ASpawnManager::ASpawnManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASpawnManager::BeginPlay()
{
	Super::BeginPlay();
	SpawnCreature(FVector::ZeroVector,FRotator::ZeroRotator,CreatureGrey);

	
}

// Called every frame
void ASpawnManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	

}

/*싱글톤 객체를 불러오기 위한 함수*/
ASpawnManager* ASpawnManager::GetInstance(UWorld* WorldContext)
{
	if(Instance == nullptr)
	{
		//싱글톤 패턴으로 써야 하나, 이미 월드에 배치되는 Actor 캐릭터이기에 불러오는 방식으로 변경
        Instance = Cast<ASpawnManager>(UGameplayStatics::GetActorOfClass(WorldContext, ASpawnManager::StaticClass()));
	}
	return Instance;
}

/*Creature를 Spawn하기 위한 SpawnActor의 Wrapper 함수*/
ACreatureBase* ASpawnManager::SpawnCreature(FVector SpawnPoint, FRotator SpawnRotate,FString Creature)
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

