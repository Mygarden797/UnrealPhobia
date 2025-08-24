// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_SetRunVelocity.generated.h"

/**
 * 
 */
UCLASS()
class PARANOID_API UBTT_SetRunVelocity : public UBTTaskNode
{
	GENERATED_BODY()
	public:
	UBTT_SetRunVelocity();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	
};
