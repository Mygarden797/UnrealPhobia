// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature/PatrolManager.h"
#include "Kismet/GameplayStatics.h"

//static 특성상 타입 선언과 정의를 다시 해주어야 한다.
APatrolManager* APatrolManager::Instance = nullptr;

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
	
}

// Called every frame
void APatrolManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

APatrolManager* APatrolManager::GetInstance(UWorld* WorldContext)
{

		//싱글톤 패턴으로 써야 하나, 이미 월드에 배치되는 Actor 캐릭터이기에 불러오는 방식으로 변경
		/*지속적인 크래시 원인으로 지목되어서 싱글톤 구조를 없앴다.*/
        Instance = Cast<APatrolManager>(UGameplayStatics::GetActorOfClass(WorldContext, APatrolManager::StaticClass()));

	return Instance;
}

int32 APatrolManager::GetPatrolPointsNum()
{
	return PatrolPoints.Num();
}

