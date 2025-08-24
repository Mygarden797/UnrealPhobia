// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature/AI/Task/BTT_SetLookTarget.h"
#include "Creature/CreatureController.h"
#include "Creature/CreatureBase.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTT_SetLookTarget::UBTT_SetLookTarget()
{
    NodeName = TEXT("SetLookTarget");   
}

EBTNodeResult::Type UBTT_SetLookTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp,NodeMemory);
    ACreatureController* CreatureController = Cast<ACreatureController>(OwnerComp.GetAIOwner()); //AIController 가져오기
    RETURN_IF_NULL2(CreatureController,EBTNodeResult::Failed)


    if(CreatureController->GetbIsDetected())
    {
        UObject* Target = OwnerComp.GetBlackboardComponent()->GetValueAsObject(LookTargetKey.SelectedKeyName);
        RETURN_IF_NULL2(Target,EBTNodeResult::Failed)

        ACharacter* TargetCharacter = Cast<ACharacter>(Target);
        RETURN_IF_NULL2(TargetCharacter,EBTNodeResult::Failed)

        CreatureController->SetFocus(TargetCharacter,EAIFocusPriority::Gameplay);
       
    }
    else
    {
        CreatureController->SetFocus(nullptr,EAIFocusPriority::Gameplay);
    }
     return EBTNodeResult::Succeeded;
}
