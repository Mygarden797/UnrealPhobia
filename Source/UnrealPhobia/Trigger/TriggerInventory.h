// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Trigger.h"
#include "TriggerMirror.h"
#include "Network/Contents/NetworkMirror.h"
#include "Components/ActorComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "TriggerInventory.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTriggerEvent, ETriggerName, TriggerName);

/*
 - Name        : UTriggerInventory
 - Description : 트리거 인벤토리를 관리하는 클래스, 인벤토리 상태를 확인하여 트리거 줍기/버리기를 수행
*/
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALPHOBIA_API UTriggerInventory : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTriggerInventory();

	FOnTriggerEvent OnTriggerActicated;

protected:
	// Called when the game starts
	virtual void
	BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void PickUp(); // 트리거 줍는 함수

	UFUNCTION(BlueprintCallable)
	void DropOff(); // 트리거 떨어뜨리는 함수

	UFUNCTION(BlueprintCallable)
	void SelectSlot(int32 Index);

	int32 SelectedIndex = 0, MaxInventorySize = 2, CurrentInventorySize = 0;
	TArray<ETriggerName> Inventory;
    TArray<int64> TriggerIDs; // 임시로 트리거 ID 저장용, 나중에 다른 형식으로 하꾸는게...?

private:
		UCameraComponent *CamComp;

	float MaxReachDistance = 400; // 최대 습득 사거리
	float ReachRadius = 20;		  // 습득 반경

	bool Reach(FHitResult &OutHR) const; // 범위 내 트리거 오브젝트 확인 및 반환, 찾기 실패 시 널포인터 반환
	bool SetCameraComponent();
};
