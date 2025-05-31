// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Navigation/PathFollowingComponent.h"
#include "BTT_MoveToActor.generated.h"

/**
 * 
 */
UCLASS()
class UNREALPHOBIA_API UBTT_MoveToActor : public UBTTaskNode
{
	GENERATED_BODY()
	
	// public :
	// 	UBTT_MoveToActor();

	// 	static const FName LastFoundLocation;
		
		
	// 	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	// 	UFUNCTION()
	// 	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);

 
	// 	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=AI)
	// 	float AcceptRadius = 100;

	// private:
	// 	UBehaviorTreeComponent* CachedOwnerComp;
	
};
