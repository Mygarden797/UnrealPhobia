// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature/Manager/PatrolManager_.h"
#include "Creature/PatrolPoint.h"
#include "EngineUtils.h"


/*초기화 함수*/
void UPatrolManager_::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    /*월드에 액터가 배치된 이후에 델리게이트를 등록하기 위한 작업*/
    FWorldDelegates::OnPostWorldInitialization.AddUObject(this,&UPatrolManager_::OnWorldInitialized);
    // UE_LOG(LogTemp,Warning, TEXT("PatrolManager is Activate"));
}

void UPatrolManager_::Deinitialize()
{
    Super::Deinitialize();
}
/*순찰 포인트를 등록하기 위한 함수*/
void UPatrolManager_::RegisterPatrolPoint(AActor* PatrolPoint)
{
    if(PatrolPoint !=nullptr)
    {
        PatrolPoints.AddUnique(PatrolPoint);
    }
}



/*PatrolManager를 등록하기 위한 델리게이트 함수*/
void UPatrolManager_::OnWorldInitialized(UWorld* World, const UWorld::InitializationValues IVS)
{
    
    for(TActorIterator<APatrolPoint> It(World);It;++It)
    {
        AActor* PatrolPoint = *It;
        RegisterPatrolPoint(PatrolPoint);
    }
    // UE_LOG(LogTemp, Warning, TEXT("PatrolManager Register %d"),PatrolPoints.Num());
}


const TArray<AActor*>& UPatrolManager_::GetPatrolPoints() const
{
    return PatrolPoints;
}

int32 UPatrolManager_::GetPatrolPointsNum()
{
    return PatrolPoints.Num();   
}
