// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Creature/CreatureState.h"
#include "CreatureAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class UNREALPHOBIA_API UCreatureAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	public:
		UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = Animation)
		float Speed;

		UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = Animation)
		ECreatureState CurrentCreatureState;

		virtual void NativeUpdateAnimation(float DeltaSeconds) override;

		class APawn* Owner;
	
};
