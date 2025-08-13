// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Delegates/DelegateCombinations.h"
#include "Http.h"
#include "Json.h"
#include "NetworkBase.h"
#include "NetworkManager.generated.h"

class AProtoPlayer;
class ACreatureBase;

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMatchRequestComplete, bool, const FString&);

UCLASS()
class UNREALPHOBIA_API UNetworkManager : public UGameInstance
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable)
    void ConnectToGameServer();

    void ConnectToGameServer(FString ServerIP, FString RoomId);

    UFUNCTION(BlueprintCallable)
    void DisconnectFromGameServer();

    UFUNCTION(BlueprintCallable)
    void HandleRecvPackets();

    void SendPacket(SendBufferRef SendBuffer);



public: //매치메이킹용 함수
    UFUNCTION(BlueprintCallable, Category = "API")
    void RequestMatch();

    // 델리게이트
    FOnMatchRequestComplete OnMatchRequestComplete;


    // 플레이어 ID, 화면 생기기전 임시용
    UPROPERTY(BlueprintReadWrite, Category = "Player")
    FString PlayerId = "1111";
    UFUNCTION(BlueprintCallable, Category = "Player")
    void SetPlayerId(const FString& NewPlayerId)
    {
        PlayerId = NewPlayerId;
    }

private:
    // API 설정, 일단 키는 하드코딩함
    FString APIBaseURL = "https://wvhq60v1id.execute-api.ap-northeast-2.amazonaws.com/default";
    FString APIKey = "Dm0NENkuCA4bpJWkq51HK6ns3M7tc4dP3BXuXFpt";
    // HTTP 응답 처리
    void OnMatchRequestResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);

    // 유틸리티 함수
    FHttpRequestRef CreateAPIRequest(const FString& Endpoint, const FString& Method = "POST");
    void LogAPIResponse(const FString& RequestType, bool bSuccess, const FString& ResponseData);

public:
    void HandleSpawn(const Protocol::ObjectInfo& PlayerInfo, bool IsMine);
    void HandleSpawn(const Protocol::S_ENTER_GAME& EnterGamePkt);
    void HandleSpawn(const Protocol::S_SPAWN& SpawnPkt);
    void HandleSpawnCreature(const Protocol::S_SPAWN_CREATURE& SpawnCreaturePkt);

    void HandleDespawn(uint64 ObjectId);
    void HandleDespawn(const Protocol::S_DESPAWN& DespawnPkt);

    void HandleMove(const Protocol::S_MOVE& MovePkt);
    void HandleCreatureBehavior(const Protocol::S_CREATURE_BEHAVIOR& CreatureBehaviorPkt);

    void HandleTimer(const Protocol::S_TIMER& TimerPkt);
    void HandleStart(const Protocol::S_START& StartPkt);

    void HandleWin(const Protocol::S_WIN& WinPkt);

public:
    // GameServer
    class FSocket* Socket;
    FString IpAddress = TEXT("14.36.7.1");
    int16 Port = 7777;
    TSharedPtr<class PacketSession> GameServerSession;

public:
    UPROPERTY(EditAnywhere)
    TSubclassOf<AProtoPlayer> OtherPlayerClass;

    AProtoPlayer* MyPlayer;

    TMap<uint64, AProtoPlayer*> Players;
    TMap<uint64, ACreatureBase*> Creatures;
    
};
