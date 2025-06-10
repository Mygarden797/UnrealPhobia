// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature/AI/Task/BTT_ResetTarget.h"
#include "Creature/CreatureController.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTT_ResetTarget::UBTT_ResetTarget()
{
    NodeName = TEXT("ResetTarget");
}

EBTNodeResult::Type UBTT_ResetTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp,NodeMemory);

    ACreatureController* CreatureController = Cast<ACreatureController>(OwnerComp.GetAIOwner()); //AIController 가져오기
    RETURN_IF_NULL2(CreatureController,EBTNodeResult::Failed)



    CreatureController->ClearFocus(EAIFocusPriority::Gameplay);


    return EBTNodeResult::Succeeded;

}


