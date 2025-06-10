// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_ResetTarget.generated.h"

/**
 * 
 */
UCLASS()
class UNREALPHOBIA_API UBTT_ResetTarget : public UBTTaskNode
{
	GENERATED_BODY()

	public:
		UBTT_ResetTarget();
	protected:
		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	
};
