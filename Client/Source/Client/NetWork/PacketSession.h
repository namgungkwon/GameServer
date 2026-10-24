// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetworkHeader.h"


class FSocket;
/**
 * 
 */
class CLIENT_API PacketSession : public TSharedFromThis<PacketSession>
{
public:
	PacketSession(FSocket* Socket);
	~PacketSession();

public:
	FSocket* GetSocket() { return Socket;}
	
public:
	void Run();
	void Disconnect();
	
public:
	UFUNCTION(BlueprintCallable)
	void HandleRecvPackets();
	void SendPacket(const SendBufferRef& SendBuffer);
	

public:
	FSocket* Socket;

	TSharedPtr<class RecvWorker> RecvWorkerThread;
	TSharedPtr<class SendWorker> SendWorkerThread;
	
	TQueue<TArray<uint8>> RecvPacketQueue;
	TQueue<SendBufferRef> SendPacketQueue;
};