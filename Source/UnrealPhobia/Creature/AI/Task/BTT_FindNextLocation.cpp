// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature/AI/Task/BTT_FindNextLocation.h"

#include "Creature/CreatureBase.h"
#include "Creature/CreatureController.h"
#include "Creature/PatrolManager.h"
#include "Creature/Manager/WorldPatrolManager.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"


UBTT_FindNextLocation::UBTT_FindNextLocation()
{
    NodeName = "FindNextLocation";
    PatrolLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_FindNextLocation, PatrolLocationKey));
    PatrolIndexKey.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_FindNextLocation, PatrolIndexKey));
}


EBTNodeResult::Type UBTT_FindNextLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

    Super::ExecuteTask(OwnerComp, NodeMemory);
    
    //해당 행동 트리가 동작하고 있는 Controller와 Pawn 가져오기
    ACreatureController* CreatureController = Cast<ACreatureController>(OwnerComp.GetAIOwner());
    RETURN_IF_NULL2(CreatureController,EBTNodeResult::Failed);
    
    ACreatureBase* Creature = Cast<ACreatureBase>(CreatureController->GetPawn());
    RETURN_IF_NULL2(Creature,EBTNodeResult::Failed);
    // UE_LOG(LogTemp,Warning,TEXT("Creature is not null"));


    UNavigationSystemV1* NaviSystem = UNavigationSystemV1::GetNavigationSystem(Creature->GetWorld());
    RETURN_IF_NULL2(NaviSystem,EBTNodeResult::Failed);
    // UE_LOG(LogTemp,Warning,TEXT("Navi is not null"));
    
    //StartLocation, 시작 위치 불러오기
    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    RETURN_IF_NULL2(Blackboard,EBTNodeResult::Failed)

    UWorldPatrolManager* PatrolManager = GetWorld()->GetSubsystem<UWorldPatrolManager>();
    RETURN_IF_NULL2(PatrolManager,EBTNodeResult::Failed)



    int32 PatrolPointsNum = PatrolManager->GetPatrolPointsNum();
    const TArray<AActor*>& PatrolArray = PatrolManager->GetPatrolPoints();
    UE_LOG(LogTemp, Display, TEXT("PatrolPoints is %d"),PatrolPointsNum);
    /* 순찰 할 포인트들이 존재할 경우*/
    if(PatrolPointsNum != 0)
    {
        int32 Index = Blackboard->GetValueAsInt(PatrolIndexKey.SelectedKeyName);
        Index = Index % PatrolPointsNum; // 초기값이나 다른 쓰레기값 방지

        RETURN_IF_NULL2(PatrolArray[Index],EBTNodeResult::Failed)
        AActor* NextPatrolPoint = PatrolArray[Index];
        RETURN_IF_NULL2(NextPatrolPoint,EBTNodeResult::Failed)

        Blackboard->SetValueAsVector(PatrolLocationKey.SelectedKeyName, NextPatrolPoint->GetActorLocation());
        Blackboard->SetValueAsInt(PatrolIndexKey.SelectedKeyName, Index % PatrolPointsNum);

        return EBTNodeResult::Succeeded;

    }
    else
    {/* 순찰할 포인트가 존재하지 않을 경우*/
        FVector StartLocation = Blackboard->GetValueAsVector(ACreatureController::StartLocation);

        FNavLocation NextPatrol;
        //시작 위치, 순찰 거리를 범위로 다음에 갈 순찰 포인트를 결정
        if(NaviSystem->GetRandomPointInNavigableRadius(StartLocation,CreatureController->PatrolDistance,NextPatrol))
        {
            OwnerComp.GetBlackboardComponent()->SetValueAsVector(ACreatureController::PatrolLocation,NextPatrol.Location);

            return EBTNodeResult::Succeeded;
        }
    }


    return EBTNodeResult::Failed;


}

