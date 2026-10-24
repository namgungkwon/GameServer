// Fill out your copyright notice in the Description page of Project Settings.


#include "PacketSession.h"

#include "NetworkWorker.h"
#include "Sockets.h"
#include "GameServerPacketHandler.h"
#include "LoginServerPacketHandler.h"
#include "SocketSubsystem.h"

PacketSession::PacketSession(FSocket* Socket) : Socket(Socket)
{
	Init_PacketHandler();
	GameServerPacketHandler::Init();
	LoginServerPacketHandler::Init();
}

PacketSession::~PacketSession()
{
	Disconnect();
}

void PacketSession::Run()
{
	RecvWorkerThread = MakeShared<RecvWorker>(Socket, AsShared());
	SendWorkerThread = MakeShared<SendWorker>(Socket, AsShared());
}

void PacketSession::HandleRecvPackets()
{
	while (true)
	{
		TArray<uint8> Packet;
		if (RecvPacketQueue.Dequeue(OUT Packet) == false)
			break;

		PacketSessionRef sessionRef = AsShared();
		LoginServerPacketHandler::HandlePacket(sessionRef, Packet.GetData(), Packet.Num());
	}
}

void PacketSession::SendPacket(const SendBufferRef& SendBuffer)
{
	SendPacketQueue.Enqueue(SendBuffer);
}

void PacketSession::Disconnect()
{
	if (RecvWorkerThread)
	{
		RecvWorkerThread->Destroy();
		RecvWorkerThread = nullptr;
	}
	if (SendWorkerThread)
	{
		SendWorkerThread->Destroy();
		SendWorkerThread = nullptr;
	}

	if (Socket)
	{
		ISocketSubsystem::Get()->DestroySocket(Socket);
		Socket = nullptr;
	}
}
