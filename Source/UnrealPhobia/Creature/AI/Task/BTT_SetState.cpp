// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_SetState.h"



UBTT_SetState::UBTT_SetState()
{
    NodeName = "SetState";
}

EBTNodeResult::Type UBTT_SetState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp,NodeMemory);
    // UE_LOG(LogTemp,Warning,TEXT("BTT_SetState is Activated"));

    ACreatureController* CreatureController = Cast<ACreatureController>(OwnerComp.GetAIOwner()); //AIController 가져오기
    if(CreatureController == nullptr) 
    {
        UE_LOG(LogTemp,Warning,TEXT("CreatureController is null"));
        return EBTNodeResult::Failed;
    }

    APawn* Pawn = CreatureController->GetPawn(); //Creatrue Pawn 가져오기
    if(Pawn == nullptr)
    {
        UE_LOG(LogTemp,Warning,TEXT("Pawn is null"));
        return EBTNodeResult::Failed;
    } 

    ACreatureBase* Creature = Cast<ACreatureBase>(Pawn); //Creature casting
    if(Creature == nullptr)
    {
        UE_LOG(LogTemp,Warning,TEXT("Creature is null"));
        return EBTNodeResult::Failed;
    } 
    // UE_LOG(LogTemp,Warning,TEXT("BTT_SetState is Still Activated"));
    Creature->SetState(CreatureState); //상태 변환

    return EBTNodeResult::Succeeded;


    
}



