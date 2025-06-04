// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "NetworkBase.h"
#include "NetworkManager.generated.h"

class AProtoPlayer;
/**
 * 
 */
UCLASS()
class UNREALPHOBIA_API UNetworkManager : public UGameInstance
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void ConnectToGameServer();

    UFUNCTION(BlueprintCallable)
    void DisconnectFromGameServer();

    UFUNCTION(BlueprintCallable)
    void HandleRecvPackets();

    void SendPacket(SendBufferRef SendBuffer);

public:
    void HandleSpawn(const Protocol::ObjectInfo& PlayerInfo, bool IsMine);
    void HandleSpawn(const Protocol::S_ENTER_GAME& EnterGamePkt);
    void HandleSpawn(const Protocol::S_SPAWN& SpawnPkt);
    void HandleSpawnCreature(const Protocol::S_SPAWN_CREATURE& SpawnCreaturePkt);

    void HandleDespawn(uint64 ObjectId);
    void HandleDespawn(const Protocol::S_DESPAWN& DespawnPkt);

    void HandleMove(const Protocol::S_MOVE& MovePkt);

    void HandleTimer(const Protocol::S_TIMER& TimerPkt);
    void HandleStart(const Protocol::S_START& StartPkt);

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
    
};
