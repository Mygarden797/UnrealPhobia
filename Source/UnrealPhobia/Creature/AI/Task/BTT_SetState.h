// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Creature/CreatureState.h"
#include "Creature/CreatureBase.h"
#include "Creature/CreatureController.h"
#include "BTT_SetState.generated.h"

/**
 * 
 */
UCLASS()
class UNREALPHOBIA_API UBTT_SetState : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
	public:
		UBTT_SetState();
		
		UPROPERTY(EditAnywhere, BlueprintReadOnly)
		ECreatureState CreatureState;
	protected:
		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	
};
