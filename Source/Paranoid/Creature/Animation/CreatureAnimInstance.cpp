// Fill out your copyright notice in the Description page of Project Settings.

#include "Creature/Animation/CreatureAnimInstance.h"
#include "Creature/CreatureController.h"
#include "Creature/CreatureBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"

void UCreatureAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);
    if (!Owner)
    {
        Owner = TryGetPawnOwner();
    }

    ACreatureBase *Creature = Cast<ACreatureBase>(Owner);
    if (!Creature)
    {
        return;
    }

    // 현재 폰의 속도 가져오기
    Speed = Creature->GetVelocity().Size();

    // 현재 폰의 상태 가져오기
    CurrentCreatureState = Creature->GetState();
}

void UCreatureAnimInstance::PlayAttackMontage()
{
    if (!GetAttackMontage())
    {
        UE_LOG(LogTemp, Display, TEXT("AttackMontage is nullptr"));
    }

    if (!Montage_IsPlaying(GetAttackMontage()))
    {
        UE_LOG(LogTemp, Display, TEXT("AttackMontage is Activate"));
        Montage_Play(GetAttackMontage(), 2.0f);
    }
}

void UCreatureAnimInstance::PlayCommunicateMontage()
{
    if (!GetCommunicateMontage())
    {
        UE_LOG(LogTemp, Display, TEXT("AttackMontage is nullptr"));
    }

    if (!Montage_IsPlaying(GetCommunicateMontage()))
    {
        UE_LOG(LogTemp, Display, TEXT("AttackMontage is Activate"));
        Montage_Play(GetCommunicateMontage());
    }
}

class UBlendSpace *UCreatureAnimInstance::GetMovementBlendspace() const
{
    RETURN_IF_NULL2(Owner, nullptr)
    ACreatureBase *Creature = Cast<ACreatureBase>(Owner);
    RETURN_IF_NULL2(Creature, nullptr)

    UCreatureAnimationDataAsset *CreatureAnimationData = Creature->GetCreatureAnimationData();
    RETURN_IF_NULL2(CreatureAnimationData, nullptr)

    return CreatureAnimationData->MovementBlendspace;
}

class UAnimMontage *UCreatureAnimInstance::GetAttackMontage() const
{
    RETURN_IF_NULL2(Owner, nullptr)
    ACreatureBase *Creature = Cast<ACreatureBase>(Owner);
    RETURN_IF_NULL2(Creature, nullptr)

    UCreatureAnimationDataAsset *CreatureAnimationData = Creature->GetCreatureAnimationData();
    RETURN_IF_NULL2(CreatureAnimationData, nullptr)

    return CreatureAnimationData->AttackMontage;
}

class UAnimMontage *UCreatureAnimInstance::GetCommunicateMontage() const
{
    RETURN_IF_NULL2(Owner, nullptr)
    ACreatureBase *Creature = Cast<ACreatureBase>(Owner);
    RETURN_IF_NULL2(Creature, nullptr)

    UCreatureAnimationDataAsset *CreatureAnimationData = Creature->GetCreatureAnimationData();
    RETURN_IF_NULL2(CreatureAnimationData, nullptr)

    return CreatureAnimationData->CommunicateMontage;
}
