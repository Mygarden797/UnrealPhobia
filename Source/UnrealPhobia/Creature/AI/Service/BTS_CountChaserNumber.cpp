// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature/AI/Service/BTS_CountChaserNumber.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NetworkPlayer.h"
#include "ChaseSystemTypes.h"

UBTS_CountChaserNumber::UBTS_CountChaserNumber()
{
    NodeName = TEXT("Handle Chase State");
    bNotifyBecomeRelevant = true;
    bCreateNodeInstance = true;
}

void UBTS_CountChaserNumber::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        UE_LOG(LogTemp, Error, TEXT("BTS_CountChaserNumber: BlackboardComponent is null"));
        return;
    }

    ANetworkPlayer* CurrentTargetPlayer = Cast<ANetworkPlayer>(BlackboardComp->GetValueAsObject(TargetKey.SelectedKeyName));

    if (CurrentTargetPlayer != LastChasedPlayer.Get())
    {
        if (LastChasedPlayer.IsValid())
        {
            LastChasedPlayer->RemoveChaser();
            UE_LOG(LogTemp, Display, TEXT("BTS_CountChaserNumber: RemoveChaser()"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("BTS_CountChaserNumber: LastChasedPlayer is null"));
        }
        if (CurrentTargetPlayer)
        {
            CurrentTargetPlayer->AddChaser();
            UE_LOG(LogTemp, Display, TEXT("BTS_CountChaserNumber: AddChaser()"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("BTS_CountChaserNumber: CurrentTargetPlayer is null"));
        }
        LastChasedPlayer = CurrentTargetPlayer;
    }
}

void UBTS_CountChaserNumber::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::OnCeaseRelevant(OwnerComp, NodeMemory);
    UE_LOG(LogTemp, Display, TEXT("CountChaserNumber: OnCeaseRelevant Entered"));

    if (LastChasedPlayer.IsValid())
    {
        LastChasedPlayer->RemoveChaser();
        UE_LOG(LogTemp, Display, TEXT("BTS_CountChaserNumber: RemoveChaser()"));
    }
    LastChasedPlayer = nullptr;
}

