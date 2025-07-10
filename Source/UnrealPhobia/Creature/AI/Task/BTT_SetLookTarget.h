// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_SetLookTarget.generated.h"

/**
 * 
 */
UCLASS()
class UNREALPHOBIA_API UBTT_SetLookTarget : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
	public:
		UBTT_SetLookTarget();

		UPROPERTY(EditAnywhere,Category = "Blackboard")
		struct FBlackboardKeySelector LookTargetKey;
	
	protected:
		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
};
