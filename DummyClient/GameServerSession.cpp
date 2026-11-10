#include "pch.h"
#include "GameServerSession.h"

#include "GameServerPacketHandler.h"

void GameServerSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef packetSession = GetPacketSessionRef();
	GameServerPacketHandler::HandlePacket(packetSession, buffer, len);
}
