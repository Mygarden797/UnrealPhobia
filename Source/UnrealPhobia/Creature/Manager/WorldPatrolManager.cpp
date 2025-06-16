// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature/Manager/WorldPatrolManager.h"
#include "Creature/PatrolPoint.h"
#include "EngineUtils.h"


/*GameInstacneSubsystem으로 하려고 했으나, 확인 결과 불가능하여 World로 바꾸게 되었다. 레벨 로드 전에 생성되어 안되는 것으로 추측*/
void UWorldPatrolManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    /*월드에 액터가 배치된 이후에 델리게이트를 등록하기 위한 작업*/
    FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &UWorldPatrolManager::OnWorldInitialized);
    
    UE_LOG(LogTemp,Warning, TEXT("WorldPatrolManager is Activate"));
}

void UWorldPatrolManager::Deinitialize()
{
    Super::Deinitialize();
    
}

void UWorldPatrolManager::RegisterPatrolPoint(AActor* PatrolPoint)
{
    if(PatrolPoint !=nullptr)
    {
        PatrolPoints.AddUnique(PatrolPoint);
    }
}

int32 UWorldPatrolManager::GetPatrolPointsNum()
{
    return PatrolPoints.Num();
}

void UWorldPatrolManager::OnWorldInitialized(UWorld* World, const UWorld::InitializationValues IVS)
{
    for(TActorIterator<APatrolPoint> It(World);It;++It)
    {
        AActor* PatrolPoint = *It;
        RegisterPatrolPoint(PatrolPoint);
    }
    UE_LOG(LogTemp, Warning, TEXT("WorldPatrolManager Register %d"),PatrolPoints.Num());
}

const TArray<AActor*>& UWorldPatrolManager::GetPatrolPoints() const
{
    return PatrolPoints;
}