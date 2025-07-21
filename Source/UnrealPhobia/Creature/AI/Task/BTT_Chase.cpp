// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature/AI/Task/BTT_Chase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Creature/CreatureBase.h"
#include "Creature/CreatureController.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "UnrealPhobia/Managers/SoundManager.h"





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
    FNavLocation ProjectedLocation;
    //2025.07.15 LastFoundLocation이 네비 메쉬위에 있도록 하기 위한 장치
    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    if (NavSys && NavSys->ProjectPointToNavigation(LastTargetLocation, ProjectedLocation, FVector(50, 50, 200))) {
        
        Blackboard->SetValueAsVector(LastFoundLocation, ProjectedLocation.Location);
    } else {
        
        Blackboard->SetValueAsVector(LastFoundLocation, LastTargetLocation);
    }

    if (UWorld* World = CreatureController->GetWorld())
    {
        auto* SoundMgr = World->GetGameInstance()->GetSubsystem<USoundManager>();
        if (SoundMgr && CreatureController->DetectSFX)
        {
            SoundMgr->PlaySFX2D(CreatureController->DetectSFX);
        }
    }

    // CreatureController->MoveToActor(TargetActor,AcceptRadius);

    return EBTNodeResult::Succeeded;


}
