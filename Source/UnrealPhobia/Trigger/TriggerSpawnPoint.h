// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "TriggerSpawnPoint.generated.h"

/*
 - Name        : ATriggerSpawnPoint
 - Description : 트리거가 임의 소환되는 지점
*/
UCLASS()
class UNREALPHOBIA_API ATriggerSpawnPoint : public ATargetPoint
{
	GENERATED_BODY()
public:
	ATriggerSpawnPoint();

	bool bCanSpawn = true;

	int32 SpawnerId = 1;
};
