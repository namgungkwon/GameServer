// Fill out your copyright notice in the Description page of Project Settings.

#include "NetworkManager.h"

#include "GameServerPacketHandler.h"
#include "PacketSession.h"
#include "SocketSubsystem.h"
#include "Common/TcpSocketBuilder.h"
#include "Protocol.pb.h"

void UNetworkManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogTemp, Log, TEXT("Network Manager Initialized"));
}

void UNetworkManager::Deinitialize()
{
	Super::Deinitialize();
}

void UNetworkManager::SendPacket(const SendBufferRef& SendBuffer)
{
	if (GameServer) GameServer->SendPacket(SendBuffer);
	else if (LoginServer) LoginServer->SendPacket(SendBuffer);
}

void UNetworkManager::Tick(float DeltaTime)
{
	if (LoginServer)
	{
		LoginServer->HandleRecvPackets();
	}
	if (GameServer)
	{
		GameServer->HandleRecvPackets();
	}
}

bool UNetworkManager::ConnectToLoginServer()
{
	if (LoginServer != nullptr)
		return false;
	
	const uint16 Port = 8001;
	FSocket* Socket = ConnectServer(FString("127.0.0.1"), Port);
	if (Socket == nullptr)
		return false;
	
	LoginServer = MakeShared<PacketSession>(Socket);
	UE_LOG(LogTemp, Log, TEXT("LoginServer Connected"));

	LoginServer->Run();
	return true;
}

bool UNetworkManager::ConnectToGameServer()
{
	if (GameServer != nullptr)
		return false;

	const uint16 Port = 8000;
	FSocket* Socket = ConnectServer(FString("127.0.0.1"), Port);
	if (Socket == nullptr)
		return false;
	
	GameServer = MakeShared<PacketSession>(Socket);
	UE_LOG(LogTemp, Log, TEXT("GameServer Connected"));

	GameServer->Run();

	Protocol::CG_ENTER_GAME Pkt;
	Pkt.set_playerid(1);
	SendBufferRef SendBuffer = GameServerPacketHandler::MakeSendBuffer(Pkt);
	GameServer->SendPacket(SendBuffer);
	return true;
}

void UNetworkManager::DisconnectFromLoginServer()
{
	if (LoginServer == nullptr)
		return;
	
	LoginServer->Disconnect();
	LoginServer = nullptr;
}

void UNetworkManager::DisconnectFromGameServer()
{
	if (GameServer == nullptr)
		return;
	GameServer->Disconnect();
	GameServer = nullptr;
}

void UNetworkManager::SendAuthTokenPkt()
{
	Protocol::CG_AUTH_TOKEN Pkt;
	Pkt.set_token(TCHAR_TO_UTF8(*token));
	SendBufferRef SendBuffer = GameServerPacketHandler::MakeSendBuffer(Pkt);
	GameServer->SendPacket(SendBuffer);
}


FSocket* UNetworkManager::ConnectServer(FString IpAddress, uint16 Port)
{
	FSocket* Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(TEXT("Stream"), TEXT("ServerSocket"));
	FIPv4Address Ip;
	FIPv4Address::Parse(IpAddress, Ip);

	TSharedPtr<FInternetAddr> InternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	InternetAddr->SetIp(Ip.Value);
	InternetAddr->SetPort(Port);

	if (Socket->Connect(*InternetAddr) == false)
	{
		ISocketSubsystem::Get()->DestroySocket(Socket);
		return nullptr;
	}

	return Socket;
}
