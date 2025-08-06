// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Protocol.pb.h"
#include "ProtoPlayer.generated.h"

UCLASS()
class UNREALPHOBIA_API AProtoPlayer : public ACharacter
{
    GENERATED_BODY()
public:
    AProtoPlayer();
    virtual ~AProtoPlayer();

protected:
    virtual void BeginPlay();
    virtual void Tick(float DeltaSeconds) override;

    UPROPERTY(BlueprintReadWrite, Category = "Stamina")
    bool bIsSprinting = false;

    UPROPERTY(BlueprintReadWrite, Category = "Stamina")
    bool bIsCrouch = false;

public:
    bool IsMyPlayer();

    Protocol::MoveState GetMoveState() { return PlayerInfo->state(); }
    void SetMoveState(Protocol::MoveState State);
    Protocol::CrouchState GetCrouchState() { return PlayerInfo->crouch(); }
    void SetCrouchState(Protocol::CrouchState State) { PlayerInfo->set_crouch(State); }

public:
    void SetPlayerInfo(const Protocol::PosInfo& Info);
    void SetDestInfo(const Protocol::PosInfo& Info);
    Protocol::PosInfo* GetPlayerInfo() { return PlayerInfo; }

protected:
    class Protocol::PosInfo* PlayerInfo; // 현재 위치
    class Protocol::PosInfo* DestInfo; // 목적지

    void SetCrouch();
    void SetSprint();
    void SetWalk();

};
