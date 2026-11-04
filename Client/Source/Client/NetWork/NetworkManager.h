// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "NetworkHeader.h"
#include "PacketSession.h"
#include "NetworkManager.generated.h"

class FSocket;
/**
 * 
 */
UCLASS()
class CLIENT_API UNetworkManager : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:
	PacketSessionRef GetLoginServer() {return LoginServer; }
	PacketSessionRef GetGameServer() { return GameServer; }
	void			SendPacket(const SendBufferRef& SendBuffer);

public:
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override {    RETURN_QUICK_DECLARE_CYCLE_STAT(UMyGSSubsystem, STATGROUP_Tickables);}
	virtual bool IsTickable() const override { return !IsTemplate();}
public:
	UFUNCTION(BlueprintCallable)
	bool ConnectToLoginServer();
	UFUNCTION(BlueprintCallable)
	bool ConnectToGameServer();

	UFUNCTION(BlueprintCallable)
	void DisconnectFromLoginServer();
	UFUNCTION(BlueprintCallable)
	void DisconnectFromGameServer();

public:
	UFUNCTION(BlueprintCallable)
	void SendAuthTokenPkt();

private:
	FSocket* ConnectServer(FString IpAddress, uint16 Port);

private:
	PacketSessionRef LoginServer;
	PacketSessionRef GameServer;

public:
	FString token = FString();
};
