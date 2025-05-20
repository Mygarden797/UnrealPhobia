// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_IncreaseInvestigateCount.generated.h"

/**
 * 
 */
UCLASS()
class UNREALPHOBIA_API UBTT_IncreaseInvestigateCount : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	public:
		UBTT_IncreaseInvestigateCount();
		
		UPROPERTY(EditAnywhere,Category = "Blackboard")
		struct FBlackboardKeySelector InvestigateCountKey;

		UPROPERTY(EditAnywhere,Category = "Blackboard")
		float MaxInvestigateCount = 4;

	protected:
		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	
};
