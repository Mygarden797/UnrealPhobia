// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature/AI/Task/BTT_Attack.h"
#include "Creature/CreatureController.h"
#include "Creature/CreatureBase.h"


UBTT_Attack::UBTT_Attack()
{
    NodeName = TEXT("Attack");
}

EBTNodeResult::Type UBTT_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);
    ACreatureController* CreatureController = Cast<ACreatureController>(OwnerComp.GetAIOwner());
    RETURN_IF_NULL2(CreatureController,EBTNodeResult::Failed);

    ACreatureBase* Creature = Cast<ACreatureBase>(CreatureController->GetPawn());
    RETURN_IF_NULL2(Creature,EBTNodeResult::Failed);

    Creature->Attack();

    return EBTNodeResult::Succeeded;
}
