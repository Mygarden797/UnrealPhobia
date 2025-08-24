// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_SetVelocity.generated.h"

/**
 * 
 */
UCLASS()
class PARANOID_API UBTT_SetVelocity : public UBTTaskNode
{
	GENERATED_BODY()

	public:
		UBTT_SetVelocity();

		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	
};
