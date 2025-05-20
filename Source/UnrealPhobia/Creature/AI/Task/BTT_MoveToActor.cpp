// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature/AI/Task/BTT_MoveToActor.h"
#include "Creature/CreatureController.h"
#include "Creature/CreatureBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"




// const FName UBTT_MoveToActor::LastFoundLocation(TEXT("LastFoundLocation"));

// UBTT_MoveToActor::UBTT_MoveToActor()
// {
//     NodeName = "MoveToActor";
// }

// EBTNodeResult::Type UBTT_MoveToActor::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
// {
//     Super::ExecuteTask(OwnerComp,NodeMemory);

//     //OwnerComp를 Cached에 저장
//     CachedOwnerComp = &OwnerComp;

//     //Controller, Blackboard, CreatureBase, TargetActor를 저장
//     ACreatureController* CreatureController = Cast<ACreatureController>(OwnerComp.GetAIOwner());
//     RETURN_IF_NULL2(CreatureController,EBTNodeResult::Failed)

//     UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
//     RETURN_IF_NULL2(BlackboardComp,EBTNodeResult::Failed)

//     ACreatureBase* Creature = Cast<ACreatureBase>(CreatureController->GetPawn());
//     RETURN_IF_NULL2(Creature,EBTNodeResult::Failed)

//     AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject("Target"));
//     RETURN_IF_NULL2(TargetActor,EBTNodeResult::Failed)

//     //BlackBoard의 LastFoundLocation에 Target의 위치 저장
//     FVector LastTargetLocation = TargetActor->GetActorLocation();
//     BlackboardComp->SetValueAsVector(LastFoundLocation,LastTargetLocation);

//     //MoveTo를 이용해서 Target까지 이동
//     FNavPathSharedPtr NavPath;
//     FAIMoveRequest MoveRequest;
//     MoveRequest.SetGoalActor(TargetActor);
//     MoveRequest.SetAcceptanceRadius(AcceptRadius);

//     FPathFollowingRequestResult Result = CreatureController->MoveTo(MoveRequest,&NavPath);

//     //ReceiveMoveComplete의 델리게이트 등록
//     if(Result.Code == EPathFollowingRequestResult::RequestSuccessful)
//     {
//         CreatureController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &UBTT_MoveToActor::OnMoveCompleted);
//         return EBTNodeResult::InProgress;
//     }



//     return EBTNodeResult::Failed;
// }

// /*결과를 처리하기 위한 델리게이트 함수*/
// void UBTTask_MoveToActor::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
// {
//     RETURN_IF_NULL(CachedOwnerComp);

//     AAIController* CreatureController = CachedOwnerComp->GetAIOwner();

//     if(CreatureController != nullptr)
//     {
//         CreatureController->ReceiveMoveCompleted.RemoveAll(this);
//     } 

//     if(Result.IsSuccess())
//     {
//         FinishLatentTask(*CachedOwnerComp,EBTNodeResult::Succeeded);
//     }
//     else
//     {
//         FinishLatentTask(*CachedOwnerComp,EBTNodeResult::Failed);
//     }

// }
