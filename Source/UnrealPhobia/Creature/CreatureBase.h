// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Creature/CreatureState.h"
#include "CreatureBase.generated.h"

//크리처 상탱 변경을 위한 델리게이트
DECLARE_MULTICAST_DELEGATE_TwoParams(FCreatureStateChangedDelegate, ECreatureState,ECreatureState);

// 크리쳐 공격 시 카메라 전환을 위한 델리게이트 (크리쳐, 렌더 타겟)
DECLARE_MULTICAST_DELEGATE_TwoParams(FCreatureAttackCameraDelegate, ACreatureBase*, UTextureRenderTarget2D*);



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

//카메라 관련
    // 카메라 시스템 컴포넌트들
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    class USceneCaptureComponent2D* AttackCamera;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    class UTextureRenderTarget2D* AttackCameraRenderTarget;

    // 공격 카메라 활성화 시간
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    float AttackCameraShowTime = 3.0f;

    // 타이머 핸들
    FTimerHandle AttackCameraTimerHandle;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//델리게이트를 위한 시그니쳐 함수 생성
	FCreatureStateChangedDelegate OnCreatureStateChanged;

    // 공격 카메라 델리게이트
    static FCreatureAttackCameraDelegate OnCreatureAttackCamera;

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


//카메라 관련
    // 공격 카메라 관련 함수들
    UFUNCTION(BlueprintCallable, Category = "Camera")
    void ActivateAttackCamera();

    UFUNCTION(BlueprintCallable, Category = "Camera")
    void DeactivateAttackCamera();

    // 렌더 타겟 반환 함수
    UFUNCTION(BlueprintCallable, Category = "Camera")
    UTextureRenderTarget2D* GetAttackCameraRenderTarget() const { return AttackCameraRenderTarget; }


private:
	float AttackDamage = 10;




//카메라 관련
    // 공격 카메라 비활성화 타이머 콜백
    void OnAttackCameraTimerEnd();

};
