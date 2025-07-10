// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_Attack.generated.h"

/**
 * 
 */
UCLASS()
class UNREALPHOBIA_API UBTS_Attack : public UBTService
{
	GENERATED_BODY()

	public:
		UBTS_Attack();
		static const FName LastFoundLocation;
		UPROPERTY(EditAnywhere,Category = "Blackboard")
		struct FBlackboardKeySelector TargetKey;

		UPROPERTY(EditAnywhere,BlueprintReadOnly, Category= "Blackboard")
		float AttackRange = 180;

	protected:
		virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
