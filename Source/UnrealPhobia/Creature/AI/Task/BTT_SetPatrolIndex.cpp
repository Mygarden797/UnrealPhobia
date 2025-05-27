// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature/AI/Task/BTT_SetPatrolIndex.h"
#include "Creature/CreatureController.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTT_SetPatrolIndex::UBTT_SetPatrolIndex()
{
    NodeName = TEXT("SetPatrolIndex");
    PatrolCountKey.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_SetPatrolIndex,PatrolCountKey));
}

EBTNodeResult::Type UBTT_SetPatrolIndex::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    RETURN_IF_NULL2(Blackboard,EBTNodeResult::Failed);


    if(PatrolCountKey.SelectedKeyName == NAME_None)
    {
        UE_LOG(LogTemp, Display, TEXT("No InvestigateCountKey"));
        return EBTNodeResult::Failed;
    }
    int32 CurrentIndex = Blackboard->GetValueAsInt(PatrolCountKey.SelectedKeyName);
    CurrentIndex++;
    Blackboard->SetValueAsInt(PatrolCountKey.SelectedKeyName,CurrentIndex);

    return EBTNodeResult::Succeeded;

}

