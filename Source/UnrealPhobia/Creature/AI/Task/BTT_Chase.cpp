// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature/AI/Task/BTT_Chase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Creature/CreatureBase.h"
#include "Creature/CreatureController.h"





const FName UBTT_Chase::LastFoundLocation(TEXT("LastFoundLocation"));

UBTT_Chase::UBTT_Chase()
{
    NodeName = TEXT("Chase");
    
}

EBTNodeResult::Type UBTT_Chase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    //Controller, Blackboard, CreatureBase, TargetActor를 저장
    ACreatureController* CreatureController = Cast<ACreatureController>(OwnerComp.GetAIOwner());
    RETURN_IF_NULL2(CreatureController,EBTNodeResult::Failed)

    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    RETURN_IF_NULL2(Blackboard,EBTNodeResult::Failed)

    ACreatureBase* Creature = Cast<ACreatureBase>(CreatureController->GetPawn());
    RETURN_IF_NULL2(Creature,EBTNodeResult::Failed)

    AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject("Target"));
    RETURN_IF_NULL2(TargetActor,EBTNodeResult::Failed)

    //BlackBoard의 LastFoundLocation에 Target의 위치 저장
    FVector LastTargetLocation = TargetActor->GetActorLocation();
    Blackboard->SetValueAsVector(LastFoundLocation,LastTargetLocation);

    // CreatureController->MoveToActor(TargetActor,AcceptRadius);

    return EBTNodeResult::Succeeded;


}
