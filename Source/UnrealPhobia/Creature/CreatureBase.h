// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Creature/CreatureState.h"


#include "CreatureBase.generated.h"

//크리처 상탱 변경을 위한 델리게이트
DECLARE_MULTICAST_DELEGATE_TwoParams(FCreatureStateChangedDelegate, ECreatureState,ECreatureState);

UCLASS()
class UNREALPHOBIA_API ACreatureBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACreatureBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Creature",meta = (AllowPrivateAccess = "true"))
	ECreatureState State;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//델리게이트를 위한 시그니쳐 함수 생성
	FCreatureStateChangedDelegate OnCreatureStateChanged;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	//상태 변경을 위한 함수
	void SetState(ECreatureState NewState);
	//상태를 가져오기 위한 변수
	ECreatureState GetState() const {return State;}

	UPROPERTY(EditInstanceOnly,BlueprintReadWrite,Category="AI")
	TArray<AActor*> PatrolPoints;

	void Attack();
	void Communicate();



private:
	float AttackDamage = 10;

};
