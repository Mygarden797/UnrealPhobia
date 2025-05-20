// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature/AI/Task/BTT_SetVelocity.h"
#include "Creature/CreatureController.h"
#include "GameFramework/CharacterMovementComponent.h"


UBTT_SetVelocity::UBTT_SetVelocity()
{
    NodeName = TEXT("SetVelocity");
}

EBTNodeResult::Type UBTT_SetVelocity::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp,NodeMemory);

    ACreatureController* CreatureController = Cast<ACreatureController>(OwnerComp.GetAIOwner());
    RETURN_IF_NULL2(CreatureController,EBTNodeResult::Failed);

    ACreatureBase* Creature = Cast<ACreatureBase>(CreatureController->GetPawn());
    RETURN_IF_NULL2(Creature,EBTNodeResult::Failed);

    Creature->GetCharacterMovement()->MaxWalkSpeed = Speed;
    
    return EBTNodeResult::Succeeded;


}

