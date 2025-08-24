// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTT_FindNextLocation.generated.h"

/**
 * 
 */
UCLASS()
class PARANOID_API UBTT_FindNextLocation : public UBTTaskNode
{
	GENERATED_BODY()
	
	public:
		UBTT_FindNextLocation();
	protected:
		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

		//현재 블랙보드 키에서 유효한 키만을 보여주게 만들기 위한 장치
		UPROPERTY(EditAnywhere,Category = "Blackboard")
		struct FBlackboardKeySelector PatrolLocationKey;

		UPROPERTY(EditAnywhere,Category = "Blackboard")
		struct FBlackboardKeySelector PatrolIndexKey;

	
};
