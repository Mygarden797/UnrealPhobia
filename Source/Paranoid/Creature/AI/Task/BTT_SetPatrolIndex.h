// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_SetPatrolIndex.generated.h"

/**
 * 
 */
UCLASS()
class PARANOID_API UBTT_SetPatrolIndex : public UBTTaskNode
{
	GENERATED_BODY()

	public:
		UBTT_SetPatrolIndex();

		UPROPERTY(EditAnywhere,Category = "Blackboard")
		struct FBlackboardKeySelector PatrolCountKey;

		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	
};
