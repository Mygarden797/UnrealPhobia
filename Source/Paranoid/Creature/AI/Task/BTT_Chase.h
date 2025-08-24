// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_Chase.generated.h"

/**
 * 
 */
UCLASS()
class PARANOID_API UBTT_Chase : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	public:
		UBTT_Chase();
		static const FName LastFoundLocation;

		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);


		UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=AI)
		float AcceptRadius = 100;

	private:
		
	
};
