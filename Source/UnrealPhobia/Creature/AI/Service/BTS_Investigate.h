// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_Investigate.generated.h"

/**
 * 
 */
UCLASS()
class UNREALPHOBIA_API UBTS_Investigate : public UBTService
{
	GENERATED_BODY()

	public:
		UBTS_Investigate();

		UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Tick)
		float InvestigateInterval = 3.0f;

		UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Tick)
		float InvestigateRandomDeviation = 0.75f;

	protected:
		virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
