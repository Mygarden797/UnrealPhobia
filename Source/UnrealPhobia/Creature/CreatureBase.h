// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Creature/CreatureState.h"
#include "Creature/CreatureDataAsset.h"
#include "Creature/Animation/CreatureAnimationDataAsset.h"
#include "Protocol.pb.h"
#include "CreatureBase.generated.h"

// 크리처 상태 변경을 위한 델리게이트
DECLARE_MULTICAST_DELEGATE_TwoParams(FCreatureStateChangedDelegate, ECreatureState, ECreatureState);

// 크리쳐 공격 시 카메라 전환을 위한 델리게이트 (크리쳐, 렌더 타겟)
DECLARE_MULTICAST_DELEGATE_TwoParams(FCreatureAttackCameraDelegate, ACreatureBase *, UTextureRenderTarget2D *);

class USpotLightComponent;
class USoundBase;

UCLASS()
class UNREALPHOBIA_API ACreatureBase : public ACharacter
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    ACreatureBase();
    ~ACreatureBase();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Creature", meta = (AllowPrivateAccess = "true"))
    ECreatureState State;

    // 카메라 관련
    //  카메라 시스템 컴포넌트들
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent *CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    class UCameraComponent *CameraComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    class USceneCaptureComponent2D *AttackCamera;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    class UTextureRenderTarget2D *AttackCameraRenderTarget;

    // 공격 카메라 활성화 시간
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    float AttackCameraShowTime = 3.0f;

    // 타이머 핸들
    FTimerHandle AttackCameraTimerHandle;

    USpotLightComponent *FaceLight;

    bool bIsFOVChanging = false;
    float FOVStart = 120.f;
    float FOVTarget = 80.f;
    float FOVDuration = 2.f;
    float FOVElapsed = 0.f;

    void StartChangeFOV(float NewFOV, float Duration);

    UPROPERTY(EditAnywhere, Category = "Audio")
    USoundBase *AttackSound;

public:
    // Called every frame
    virtual void
    Tick(float DeltaTime) override;
    // 델리게이트를 위한 시그니쳐 함수 생성
    FCreatureStateChangedDelegate OnCreatureStateChanged;

    // 공격 카메라 델리게이트
    static FCreatureAttackCameraDelegate OnCreatureAttackCamera;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;
    // 상태 변경을 위한 함수
    void SetState(ECreatureState NewState);
    // 상태를 가져오기 위한 변수
    ECreatureState GetState() const { return State; }

    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI")
    TArray<AActor *> PatrolPoints;

    void Attack();
    void Communicate();

    // 카메라 관련
    //  공격 대상의 카메라를 크리쳐의 카메라로 변환하는 함수
    void LocateTargetCamera();

    void RevertTargetCamera();

    // 공격 카메라 관련 함수들
    UFUNCTION(BlueprintCallable, Category = "Camera")
    void ActivateAttackCamera();

    UFUNCTION(BlueprintCallable, Category = "Camera")
    void DeactivateAttackCamera();

    // 렌더 타겟 반환 함수
    UFUNCTION(BlueprintCallable, Category = "Camera")
    UTextureRenderTarget2D *GetAttackCameraRenderTarget() const
    {
        return AttackCameraRenderTarget;
    }

    //멀티 이동관련
    const float MOVE_PACKET_SEND_DELAY = 0.2f;
    float MovePacketSendTimer = MOVE_PACKET_SEND_DELAY;

    bool bHaveAIController = true; // AI 컨트롤러가 있는지 여부
    void MultiBehaveior(const Protocol::PosInfo& PosInfo);

    Protocol::MoveState GetCreatureState() { return CurrentInfo->state(); }
    void SetCreatureState(Protocol::CreatureState State);
    uint64 ICreatureObjectId = 0;
    void SetObjectId(uint64 ObjectId);
    void SetCurrentInfo(const Protocol::PosInfo& Info);
    void SetDestInfo(const Protocol::PosInfo& Info);
    Protocol::PosInfo* GetCurrentInfo() { return CurrentInfo; }

private:
    float AttackDamage = 10;
    AActor *AttackTarget;

    // 크리쳐 데이터
protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Creature")
    UCreatureDataAsset *CreatureData;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Creature")
    UCreatureAnimationDataAsset *CreatureAnimationData;

    class Protocol::PosInfo* CurrentInfo; // 현재 위치
    class Protocol::PosInfo* DestInfo; // 목적지


public:
    UCreatureDataAsset *GetCreatureData() const { return CreatureData; }

    UCreatureAnimationDataAsset *GetCreatureAnimationData() const { return CreatureAnimationData; }

    // 카메라 관련
    //  공격 카메라 비활성화 타이머 콜백
    void OnAttackCameraTimerEnd();
};
