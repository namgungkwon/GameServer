// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Client/NetWork/NetworkHeader.h"
#include "Client/Network/SendBuffer.h"
#include "Client/Network/GameServerPacketHandler.h"
#include "Client/Network/NetworkManager.h"
#include "Client/NetWork/PacketSession.h"
#include "Engine/World.h"

#define SEND_PACKET(Pkt)															\
SendBufferRef SendBuffer = GameServerPacketHandler::MakeSendBuffer(Pkt);			\
GWorld->GetGameInstance()->GetSubsystem<UNetworkManager>()->SendPacket(SendBuffer);