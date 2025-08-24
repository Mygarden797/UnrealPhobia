// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CreatureDataComponent.generated.h"


/* 블루프린트 스폰을 가능하게 하기 위해서 만들어졌다., Group은 그룹화를 위해서, meta는 컴포넌트 스폰을 위해서*/

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PARANOID_API UCreatureDataComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCreatureDataComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
