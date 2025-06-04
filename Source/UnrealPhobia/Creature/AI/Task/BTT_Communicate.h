// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_Communicate.generated.h"

/**
 * 
 */
UCLASS()
class UNREALPHOBIA_API UBTT_Communicate : public UBTTaskNode
{
	GENERATED_BODY()

	public:
		UBTT_Communicate();

		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	
};
