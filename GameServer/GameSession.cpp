#include "pch.h"
#include "GameSession.h"

#include "ClientPacketHandler.h"

void GameSession::OnConnected()
{
	// temp
	_accountDBId = 1;
}

void GameSession::OnDisconnected()
{

}

void GameSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef session = GetPacketSessionRef();
	ClientPacketHandler::HandlePacket(session, buffer, len);
}
