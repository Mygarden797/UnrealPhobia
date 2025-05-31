// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature/AI/Task/BTT_Investigate.h"

#include "Creature/CreatureBase.h"
#include "Creature/CreatureController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UBTT_Investigate::UBTT_Investigate()
{
    NodeName = "Investigate";
}

EBTNodeResult::Type UBTT_Investigate::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);
    
    //해당 행동 트리가 동작하고 있는 Controller와 Pawn 가져오기
    ACreatureController* CreatureController = Cast<ACreatureController>(OwnerComp.GetAIOwner());
    RETURN_IF_NULL2(CreatureController,EBTNodeResult::Failed);
    
    ACreatureBase* Creature = Cast<ACreatureBase>(CreatureController->GetPawn());
    RETURN_IF_NULL2(Creature,EBTNodeResult::Failed);

    UNavigationSystemV1* NaviSystem = UNavigationSystemV1::GetNavigationSystem(Creature->GetWorld());
    RETURN_IF_NULL2(NaviSystem,EBTNodeResult::Failed);
    
    //StartLocation, 시작 위치 불러오기
    FVector LastFoundLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(ACreatureController::LastFoundLocation);

    FNavLocation NextPatrol;
    //주변 거리를 짧게 하여 조사 시작
    if(NaviSystem->GetRandomPointInNavigableRadius(LastFoundLocation,CreatureController->PatrolDistance,NextPatrol))
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsVector(ACreatureController::PatrolLocation,NextPatrol.Location);

        return EBTNodeResult::Succeeded;
    }
    
    
    return EBTNodeResult::Failed;
}
