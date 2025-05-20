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

    RETURN_IF_NULL(Owner);
    ACreatureBase* Creature = Cast<ACreatureBase>(Owner);
    RETURN_IF_NULL(Creature)

    //현재 폰의 속도 가져오기
    Speed = Creature->GetVelocity().Size();
    
    //현재 폰의 상태 가져오기
    CurrentCreatureState = Creature->GetState();

    

}
