// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature/Animation/CreatureAnimInstance.h"
#include "Creature/CreatureController.h"
#include "Creature/CreatureBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"


void UCreatureAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);
    if(!Owner)
    {
        Owner = TryGetPawnOwner();
    }
    

    ACreatureBase* Creature = Cast<ACreatureBase>(Owner);
    if(!Creature)
    {
        return;
    }

    //현재 폰의 속도 가져오기
    Speed = Creature->GetVelocity().Size();
    
    //현재 폰의 상태 가져오기
    CurrentCreatureState = Creature->GetState();

    

}

void UCreatureAnimInstance::PlayAttackMontage()
{
    if(!AttackMontage)
    {
        UE_LOG(LogTemp, Display, TEXT("AttackMontage is nullptr"));
    }
    
    if(!Montage_IsPlaying(AttackMontage))
    {
        UE_LOG(LogTemp, Display, TEXT("AttackMontage is Activate"));
        Montage_Play(AttackMontage);
    }
}

void UCreatureAnimInstance::PlayCommunicateMontage()
{
    if(!CommunicateMontage)
    {
        UE_LOG(LogTemp, Display, TEXT("AttackMontage is nullptr"));
    }
    
    if(!Montage_IsPlaying(CommunicateMontage))
    {
        UE_LOG(LogTemp, Display, TEXT("AttackMontage is Activate"));
        Montage_Play(CommunicateMontage);
    }
}
