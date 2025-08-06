// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CreatureBase.h"
#include "CreatureState.h"
#include "UnrealPhobia/Assets/AudioAssets.h"

#include "CreatureController.generated.h"



//NULL을 돌려주는 매크로
#define RETURN_IF_NULL(Ptr) \
    if ((Ptr) == nullptr) \
    { \
        UE_LOG(LogTemp, Warning, TEXT("[%s] %s is nullptr"), *GetNameSafe(this), TEXT(#Ptr)); \
        return; \
    }

#define RETURN_IF_NULL2(Ptr, Ret) \
    if ((Ptr) == nullptr) \
    { \
        UE_LOG(LogTemp, Warning, TEXT("[%s] %s is nullptr"), *GetNameSafe(this), TEXT(#Ptr)); \
        return Ret; \
    }


/**
 * 
 */
UCLASS()
class UNREALPHOBIA_API ACreatureController : public AAIController
{
	GENERATED_BODY()

	public: 
		ACreatureController();

		virtual void OnPossess(APawn * PawnToPossess) override;  
		// virtual void OnUnPossess() override; 

		static const FName StartLocation; //초기 위치 표현
		static const FName PatrolLocation; //다음 수색 장소 표현
		static const FName Target; // 추적 대상 표현
		static const FName CreatureState; //현재 몬스터 상태 표현
		static const FName LastFoundLocation; // 마지막 발견 장소
		static const FName Friend;
		/*2025-07-18 CreatureDataAsset 추가로 삭제*/
		// UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= Creature)
		// float CreatureSightRadius;

		// UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= Creature)
		// float CreatureLoseSightRadius;

		// UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= Creature)
		// float CreaturePeripheralVisionAngleDegrees;

		// UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= Creature)
		// float CreatureHearingRange = 2500.f;

		// UPROPERTY(EditAnywhere,Category=Creauture)
		// float InvestigateDistance = 50;

		// UPROPERTY(EditAnywhere,Category =Creature)
		// float PatrolDistance = 200;

		bool GetbIsDetected() {return bIsDetected;}

		virtual void PostInitializeComponents() override;
		virtual void BeginPlay() override;

		// virtual FGenericTeamId GetGenericTeamId() const override;


	private:
		UFUNCTION()
		void OnTargetDetected(AActor* Actor, FAIStimulus Stimulus);

		UFUNCTION()
		void OnFriendDetected(AActor* Actor, FAIStimulus Stimulus);


		UPROPERTY(EditDefaultsOnly, Category = AI)
		class UBehaviorTree* BTAsset;

		UPROPERTY(EditDefaultsOnly, Category = AI)
		class UBlackboardData* BBAsset;



		bool bIsDetected = false;

		UPROPERTY(EditDefaultsOnly, Category = Friend)
		TSet<ACreatureBase*> FriendList; 
	/* 데이터 에셋 */
	protected:
		FGenericTeamId TeamID;
		



};
