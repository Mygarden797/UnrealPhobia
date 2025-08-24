// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Trigger.h"
#include "TriggerInventory.h"
#include "TriggerSpawnPoint.h"
#include "GameFramework/PlayerState.h"
#include "TriggerSpawnManager.generated.h"
/*
 - Name        : ATriggerSpawnManager
 - Description : 트리거 소환을 관리하는 클래스
*/
UCLASS()
class UNREALPHOBIA_API ATriggerSpawnManager : public APlayerState
{
	GENERATED_BODY()

public:
	ATriggerSpawnManager();
	bool UpdateAvailablePoints();

	void StartSpawn();

protected:
	virtual void BeginPlay() override;

	// 기본 트리거 개수
	UPROPERTY(EditAnywhere)
	int32 DefaultTriggerCount = 62;
	// 현재 트리거 개수
	int32 CurrentTriggerCount = DefaultTriggerCount;
	FTimerHandle CooldownTimerHandle; // Timer Handle

	bool bIsSpawning = false;
	// 스폰 주기 (초)
	float SpawnCooldown = 5.0f;

	void StartCooldown();
	void EndCooldown();

	// 스폰 포인트 목록
	TArray<ATriggerSpawnPoint *> SpawnPoints;
	// 현재 스폰 가능한 스폰 포인트 목록
	TArray<ATriggerSpawnPoint *> AvailablePoints;
	// 현재 월드에 존재하는 스폰 포인트 저장
	uint8 SetTriggerSpawnPoints();

	// 스폰 포인트 목록 중 임의 소환, 성공 여부 반환
	bool SpawnTriggerInRandom();
	// 스폰 포인트 목록 중 지정 소환, 성공 여부 반환
	bool SpawnTrigger(int32 index);
};
