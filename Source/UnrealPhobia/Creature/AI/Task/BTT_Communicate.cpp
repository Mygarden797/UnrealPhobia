// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature/AI/Task/BTT_Communicate.h"
#include "Creature/CreatureController.h"
#include "Creature/CreatureBase.h"


UBTT_Communicate::UBTT_Communicate()
{
    NodeName = TEXT("Communicate");
}

EBTNodeResult::Type UBTT_Communicate::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    ACreatureController* CreatureController = Cast<ACreatureController>(OwnerComp.GetAIOwner());
    RETURN_IF_NULL2(CreatureController,EBTNodeResult::Failed)

    ACreatureBase* Creature = Cast<ACreatureBase>(CreatureController->GetPawn());
    RETURN_IF_NULL2(Creature,EBTNodeResult::Failed)

    Creature->Communicate();
    return EBTNodeResult::Succeeded;
}
