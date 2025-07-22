// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature/AI/Task/BTT_SetRunVelocity.h"
#include "Creature/CreatureController.h"
#include "GameFramework/CharacterMovementComponent.h"


UBTT_SetRunVelocity::UBTT_SetRunVelocity()
{
    NodeName = TEXT("SetRunVelocity");
}

EBTNodeResult::Type UBTT_SetRunVelocity::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp,NodeMemory);

    ACreatureController* CreatureController = Cast<ACreatureController>(OwnerComp.GetAIOwner());
    RETURN_IF_NULL2(CreatureController,EBTNodeResult::Failed);

    ACreatureBase* Creature = Cast<ACreatureBase>(CreatureController->GetPawn());
    RETURN_IF_NULL2(Creature,EBTNodeResult::Failed);

    UCreatureDataAsset* CreatureData = Creature->GetCreatureData();
    RETURN_IF_NULL2(CreatureData,EBTNodeResult::Failed)

    Creature->GetCharacterMovement()->MaxWalkSpeed = CreatureData->RunSpeed;
    
    return EBTNodeResult::Succeeded;
}
