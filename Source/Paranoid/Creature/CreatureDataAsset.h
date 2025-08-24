// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CreatureDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class PARANOID_API UCreatureDataAsset : public UDataAsset
{
	GENERATED_BODY()

	public:
		//시야 범위
		UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= Creature)
		float CreatureSightRadius;
		//감지 유지 거리
		UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= Creature)
		float CreatureLoseSightRadius;
		//시야 감지 각도
		UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= Creature)
		float CreaturePeripheralVisionAngleDegrees;
		//청각 감지 범위
		UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= Creature)
		float CreatureHearingRange;
		//조사 거리
		
		UPROPERTY(EditAnywhere,Category=Creauture)
		float InvestigateDistance = 50;
		//순찰 예비거리(PatrolManager가 작동하지 않을 때 작동)
		
		UPROPERTY(EditAnywhere,Category =Creature)
		float PatrolDistance = 200;
		//걷기 속도
		UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= Creature)
		float RunSpeed;

		//달리기 속도
		UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= Creature)
		float WalkSpeed;
		
		//공격범위
		UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= Creature)
		float AttackRange;
	
};
