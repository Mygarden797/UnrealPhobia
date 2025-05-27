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

	bool bCanSpawn = false;

	/*
	추후에 플레이어와 스폰 포인트가 일치하는지 확인하는 로직 추가 예정
	->다른 플레이어의 스폰 포인트와 구분하기 위해 사용됨
	*/
	int32 SpawnerId = 10;
};
