#include "pch.h"
#include "LoginServerSession.h"

#include "LoginServerPacketHandler.h"

void LoginServerSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef packetSession = GetPacketSessionRef();
	LoginServerPacketHandler::HandlePacket(packetSession, buffer, len);
}
