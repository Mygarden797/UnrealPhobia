// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CreatureAnimationDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType,Blueprintable)
class UNREALPHOBIA_API UCreatureAnimationDataAsset : public UDataAsset
{
	GENERATED_BODY()

	public:
		//속도와 방향에 따라 따로 걷기 위한 클래스
		UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,Category = Animation)
		UBlendSpace* MovementBlendspace;

		UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,Category = Animation)
		UAnimMontage* AttackMontage;

		UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = Animation)
		UAnimMontage* CommunicateMontage;
	
};
