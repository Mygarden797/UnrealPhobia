// Fill out your copyright notice in the Description page of Project Settings.


#include "CreatureBase.h"
#include "Creature/CreatureController.h"
#include "Components/CapsuleComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

// Sets default values
ACreatureBase::ACreatureBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//폰의 이름으로 프리셋 설정
	GetCapsuleComponent()-> SetCollisionProfileName("Pawn");
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	//레벨에 직접 배치되거나 런타임에 스폰 시에 자동 소유
	State = ECreatureState::Idle;
}

// Called when the game starts or when spawned
void ACreatureBase::BeginPlay()
{
	Super::BeginPlay();
	SetState(State);
	
}

// Called every frame
void ACreatureBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACreatureBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

//새로운 상태로 설정하는 함수
void ACreatureBase::SetState(ECreatureState NewState)
{
	// UE_LOG(LogTemp, Warning, TEXT("SetState called with %d"), (uint8)NewState);
	//새 상태와 예전 상태를 저장
	ECreatureState OldState = State;
	State = NewState;

	ACreatureController* CreatureController = Cast<ACreatureController>(GetController());
	//블랙보드 함수 설정
	if (CreatureController && CreatureController->GetBlackboardComponent())
	{
		// UE_LOG(LogTemp, Warning, TEXT("Setting Blackboard State to %d"), uint8(State));
		CreatureController->GetBlackboardComponent()->SetValueAsEnum(ACreatureController::CreatureState, uint8(State));
	}
	else
	{
		// UE_LOG(LogTemp, Error, TEXT("CreatureController or BlackboardComponent is null!"));
	}
	//델리게이트 함수 발동
	OnCreatureStateChanged.Broadcast(OldState, NewState);

}

