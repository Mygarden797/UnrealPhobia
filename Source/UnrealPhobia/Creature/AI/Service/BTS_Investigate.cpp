// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature/AI/Service/BTS_Investigate.h"
#include "Creature/CreatureController.h"
#include "Creature/CreatureBase.h"
#include "Creature/CreatureState.h"


UBTS_Investigate::UBTS_Investigate()
{
    NodeName = TEXT("InvestigateTimer");
    bNotifyTick = true;
    Interval = InvestigateInterval;
    RandomDeviation = InvestigateRandomDeviation;
    
}

void UBTS_Investigate::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp,NodeMemory,DeltaSeconds);

    UE_LOG(LogTemp, Display, TEXT("Tick is Activate"));
    ACreatureController* CreatureController = Cast<ACreatureController>(OwnerComp.GetAIOwner());
    RETURN_IF_NULL(CreatureController);

    ACreatureBase* Creature = Cast<ACreatureBase>(CreatureController->GetPawn());
    RETURN_IF_NULL(Creature);

    ECreatureState CurrentState = Creature->GetState();

    if(CurrentState == ECreatureState::Patrol)
    {
        Creature->SetState(ECreatureState::Investigate);
    }
    
}
