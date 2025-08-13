// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkManager.h"
#include "Sockets.h"
#include "Common/TcpSocketBuilder.h"
#include "Serialization/ArrayWriter.h"
#include "SocketSubsystem.h"
#include "PacketSession.h"
#include "Protocol.pb.h"
#include "Network/Contents/NetworkPlayer.h"
#include "ClientPacketHandler.h"
#include "Contents/ProtoPlayer.h"
#include "ProtoGameState.h"
#include "Creature/CreatureBase.h"

void UNetworkManager::ConnectToGameServer()
{
    Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(TEXT("Stream"), TEXT("Client Socket"));

    FIPv4Address Ip;
    FIPv4Address::Parse(IpAddress, Ip);

    TSharedRef<FInternetAddr> InternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
    InternetAddr->SetIp(Ip.Value);
    InternetAddr->SetPort(Port);

    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connecting To Server...")));

    bool Connected = Socket->Connect(*InternetAddr);

    if (Connected)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connection Success")));

        // Session
        GameServerSession = MakeShared<PacketSession>(Socket);
        GameServerSession->Run();

        // TEMP : Lobby���� ĳ���� ����â ��
        {
            Protocol::C_LOGIN Pkt;
            SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(Pkt);
            SendPacket(SendBuffer);
        }
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connection Failed")));
    }
}

//매치메이킹전용
void UNetworkManager::ConnectToGameServer(FString ServerIP, FString RoomId)
{
    Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(TEXT("Stream"), TEXT("Client Socket"));

    FIPv4Address Ip;
    FIPv4Address::Parse(ServerIP, Ip);

    TSharedRef<FInternetAddr> InternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
    InternetAddr->SetIp(Ip.Value);
    InternetAddr->SetPort(Port);

    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connecting To Server...")));

    bool Connected = Socket->Connect(*InternetAddr);

    if (Connected)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connection Success")));

        // Session
        GameServerSession = MakeShared<PacketSession>(Socket);
        GameServerSession->Run();

        // TEMP : Lobby���� ĳ���� ����â ��
        {
            Protocol::C_MATCH Pkt;
            const TCHAR* Str = *RoomId;
            uint64 ParsedValue = FCString::Strtoui64(Str, nullptr, 10);
            Pkt.set_room_id(ParsedValue); 
            SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(Pkt);
            SendPacket(SendBuffer);
        }
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connection Failed")));
    }
}

void UNetworkManager::DisconnectFromGameServer()
{
    if (Socket == nullptr || GameServerSession == nullptr)
        return;

    Protocol::C_LEAVE_GAME LeavePkt;
    SEND_PACKET(LeavePkt);

}

void UNetworkManager::HandleRecvPackets()
{
    if (Socket == nullptr || GameServerSession == nullptr)
        return;

    GameServerSession->HandleRecvPackets();
}

void UNetworkManager::SendPacket(SendBufferRef SendBuffer)
{
    if (Socket == nullptr || GameServerSession == nullptr)
        return;

    GameServerSession->SendPacket(SendBuffer);
}

void UNetworkManager::HandleSpawn(const Protocol::ObjectInfo& ObjectInfo, bool IsMine)
{
    if (Socket == nullptr || GameServerSession == nullptr)
        return;

    auto* World = GetWorld();
    if (World == nullptr)
        return;

    // �ߺ� ó�� üũ
    const uint64 ObjectId = ObjectInfo.object_id();
    if (Players.Find(ObjectId) != nullptr)
        return;

    FVector SpawnLocation(ObjectInfo.pos_info().x(), ObjectInfo.pos_info().y(), ObjectInfo.pos_info().z());

    if (IsMine)
    {
        auto* PC = UGameplayStatics::GetPlayerController(this, 0);
        AProtoPlayer* Player = Cast<AProtoPlayer>(PC->GetPawn());
        if (Player == nullptr)
            return;

        Player->SetPlayerInfo(ObjectInfo.pos_info());
        MyPlayer = Player;
        MyPlayer->MyPlayerId = ObjectId;
        Players.Add(ObjectInfo.object_id(), Player);
    }
    else
    {
        // 또는 충돌 처리 옵션 추가
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
        AProtoPlayer* Player = World->SpawnActor<AProtoPlayer>(OtherPlayerClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
        //AProtoPlayer* Player = Cast<AProtoPlayer>(World->SpawnActor(OtherPlayerClass, &SpawnLocation));
        Player->SetPlayerInfo(ObjectInfo.pos_info());
        Players.Add(ObjectInfo.object_id(), Player);
    }
}

void UNetworkManager::HandleSpawn(const Protocol::S_ENTER_GAME& EnterGamePkt)
{
    HandleSpawn(EnterGamePkt.player(), true);
}


void UNetworkManager::HandleSpawn(const Protocol::S_SPAWN& SpawnPkt)
{
    for (auto& Player : SpawnPkt.players())
    {
        HandleSpawn(Player, false);
    }
    for (auto& Creature : SpawnPkt.creatures())
    {
        //****소환 가져오기에서 수정 필요
        auto* World = GetWorld();
        if (World == nullptr)
            return;

        bool bAIContol;
        if (MyPlayer->MyPlayerId % 2 == Creature.creature_info().creature_control()%2)
            bAIContol = true;
        else
            bAIContol = false;

        auto* GameState = Cast<AProtoGameState>(World->GetGameState());
        GameState->SpawnCreature(
            FVector(Creature.pos_info().x(), Creature.pos_info().y(), Creature.pos_info().z()),
            FRotator(0.0f, Creature.pos_info().yaw(), 0.0f),
            Creature.creature_info().creature_type(),
            bAIContol
        );
    }
}

void UNetworkManager::HandleDespawn(uint64 ObjectId)
{
    if (Socket == nullptr || GameServerSession == nullptr)
        return;

    auto* World = GetWorld();
    if (World == nullptr)
        return;

    AProtoPlayer** FindActor = Players.Find(ObjectId);
    if (FindActor == nullptr)
        return;

    World->DestroyActor(*FindActor);
}


void UNetworkManager::HandleDespawn(const Protocol::S_DESPAWN& DespawnPkt)
{
    for (auto& ObjectId : DespawnPkt.object_ids())
    {
        HandleDespawn(ObjectId);
    }
}

void UNetworkManager::HandleMove(const Protocol::S_MOVE& MovePkt)
{
    if (Socket == nullptr || GameServerSession == nullptr)
        return;

    auto* World = GetWorld();
    if (World == nullptr)
        return;

    const uint64 ObjectId = MovePkt.info().object_id();
    AProtoPlayer** FindActor = Players.Find(ObjectId);
    if (FindActor == nullptr)
        return;

    AProtoPlayer* Player = (*FindActor);
    if (Player->IsMyPlayer())
        return;

    const Protocol::PosInfo& Info = MovePkt.info();
    //Player->SetPlayerInfo(Info);
    Player->SetDestInfo(Info);
}

void UNetworkManager::HandleCreatureBehavior(const Protocol::S_CREATURE_BEHAVIOR& MovePkt)
{
    if (Socket == nullptr || GameServerSession == nullptr)
        return;

    auto* World = GetWorld();
    if (World == nullptr)
        return;

    const uint64 ObjectId = MovePkt.info().object_id();
    ACreatureBase** FindActor = Creatures.Find(ObjectId);
    if (FindActor == nullptr)
        return;

    ACreatureBase* Creature = (*FindActor);

    const Protocol::PosInfo& Info = MovePkt.info();
    //Player->SetPlayerInfo(Info);
    
    //크리쳐 행동 함수 여기에
}

//Ÿ�̸� ����
void UNetworkManager::HandleTimer(const Protocol::S_TIMER& TimerPkt)
{
    const uint64 Timer = TimerPkt.timer();
    UE_LOG(LogTemp, Log, TEXT("Timer : %llu"), Timer);
}

//SpawnTrigger로 바꾸면 좋을 듯
void UNetworkManager::HandleStart(const Protocol::S_START& StartPkt)
{
    if (Socket == nullptr || GameServerSession == nullptr)
        return;

    auto* World = GetWorld();
    if (World == nullptr)
        return;

    auto* GameState = Cast<AProtoGameState>(World->GetGameState());

    for (auto& Trigger : StartPkt.triggers())
    {
        GameState->SpawnTrigger(Trigger);
    }
}

//매치 요청
void UNetworkManager::RequestMatch()
{
    UE_LOG(LogTemp, Log, TEXT("Requesting match%s"));

    FHttpRequestRef Request = CreateAPIRequest("/Match");

    // JSON 페이로드 생성
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
    JsonObject->SetStringField("player_id", PlayerId);
    JsonObject->SetStringField("operation", "match_request");
    JsonObject->SetStringField("timestamp", FDateTime::Now().ToString());

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
    Request->SetContentAsString(OutputString);

    // 응답 바인딩
    Request->OnProcessRequestComplete().BindUObject(this, &UNetworkManager::OnMatchRequestResponse);
    Request->ProcessRequest();
}

FHttpRequestRef UNetworkManager::CreateAPIRequest(const FString& Endpoint, const FString& Method)
{
    FHttpRequestRef Request = FHttpModule::Get().CreateRequest();

    Request->SetVerb(Method);
    Request->SetURL(APIBaseURL + Endpoint);
    Request->SetHeader("Content-Type", "application/json");
    Request->SetHeader("x-api-key", APIKey);
    Request->SetTimeout(30.0f); // 30초 타임아웃

    return Request;
}

void UNetworkManager::OnMatchRequestResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
    FString ResponseData = "";
    bool bRequestSuccess = false;

    if (bSuccess && Response.IsValid())
    {
        int32 ResponseCode = Response->GetResponseCode();
        ResponseData = Response->GetContentAsString();

        if (ResponseCode == 200)
        {
            bRequestSuccess = true;

            // JSON 파싱하여 서버 정보 추출
            TSharedPtr<FJsonObject> JsonObject;
            TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseData);

            if (FJsonSerializer::Deserialize(Reader, JsonObject))
            {
                FString ServerIP = JsonObject->GetStringField("server_ip");
                FString RoomId = JsonObject->GetStringField("room_id");
                FString Message = JsonObject->GetStringField("message");

                UE_LOG(LogTemp, Log, TEXT("Match found! room ID: %s, Message: %s"), *RoomId, *Message);

                // 여기서 게임서버 연결 로직 추가 가능
                ConnectToGameServer(ServerIP, RoomId);
            }
        }
    }

    LogAPIResponse("MatchRequest", bRequestSuccess, ResponseData);

    // 델리게이트 브로드캐스트
    OnMatchRequestComplete.Broadcast(bRequestSuccess, ResponseData);
}

void UNetworkManager::LogAPIResponse(const FString& RequestType, bool bSuccess, const FString& ResponseData)
{
    if (bSuccess)
    {
        UE_LOG(LogTemp, Log, TEXT("%s Success: %s"), *RequestType, *ResponseData);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("%s Failed: %s"), *RequestType, *ResponseData);
    }
}

void UNetworkManager::HandleWin(const Protocol::S_WIN& WinPkt)
{
    ANetworkPlayer* WinPlayer;
    WinPlayer = Cast<ANetworkPlayer>(MyPlayer);
    WinPlayer->GameWin();
}