// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature/AI/Service/BTS_Attack.h"
#include "GameFramework/Actor.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Creature/CreatureController.h"
#include "Creature/CreatureBase.h"
#include "Creature/CreatureState.h"




UBTS_Attack::UBTS_Attack()
{
    NodeName="Attack";
    TargetKey.AddObjectFilter(this,GET_MEMBER_NAME_CHECKED(UBTS_Attack,TargetKey),AActor::StaticClass());
}

void UBTS_Attack::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp,NodeMemory,DeltaSeconds);
    ACreatureController* CreatureController = Cast<ACreatureController>(OwnerComp.GetAIOwner());
    RETURN_IF_NULL(CreatureController);

    ACreatureBase* Creature = Cast<ACreatureBase>(CreatureController->GetPawn());
    RETURN_IF_NULL(Creature);

    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    RETURN_IF_NULL(Blackboard)

    AActor* Target = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));

    float Distance = Creature->GetDistanceTo(Target);
    UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), Distance);
    if(Distance <= AttackRange)
    {
        Creature->SetState(ECreatureState::Attack);
    }


    

}
