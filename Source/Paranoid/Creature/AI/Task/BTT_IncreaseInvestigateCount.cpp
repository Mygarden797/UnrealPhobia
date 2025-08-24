// Fill out your copyright notice in the Description page of Project Settings.

#include "Creature/AI/Task/BTT_IncreaseInvestigateCount.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Creature/CreatureController.h"




UBTT_IncreaseInvestigateCount::UBTT_IncreaseInvestigateCount()
{
    NodeName = TEXT("IncreaseInvestigateCount");
    InvestigateCountKey.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_IncreaseInvestigateCount,InvestigateCountKey));
}

EBTNodeResult::Type UBTT_IncreaseInvestigateCount::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp,NodeMemory);

    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    RETURN_IF_NULL2(Blackboard,EBTNodeResult::Failed);

    if(InvestigateCountKey.SelectedKeyName == NAME_None)
    {
        UE_LOG(LogTemp, Display, TEXT("No InvestigateCountKey"));
        return EBTNodeResult::Failed;
    }
    int32 CurrentInvestigate = Blackboard->GetValueAsInt(InvestigateCountKey.SelectedKeyName);

    if(MaxInvestigateCount >CurrentInvestigate)
    {
        CurrentInvestigate++;
        Blackboard->SetValueAsInt(InvestigateCountKey.SelectedKeyName,CurrentInvestigate);
        
        return EBTNodeResult::Failed;
    }
    else
    {
        CurrentInvestigate = 0;
        Blackboard->SetValueAsInt(InvestigateCountKey.SelectedKeyName,CurrentInvestigate);
        return EBTNodeResult::Succeeded;
    }
    
}
