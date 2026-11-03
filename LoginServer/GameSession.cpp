#include "pch.h"
#include "GameSession.h"

#include "SessionManager.h"

#include "ClientPacketHandler.h"


///////////////////////////////////////////////////////
//	<	ClientSession	>
///////////////////////////////////////////////////////
void GameSession::OnConnected()
{
}

void GameSession::OnDisconnected()
{

}

void GameSession::OnRecvPacket(BYTE * buffer, int32 len)
{
	PacketSessionRef session = GetPacketSessionRef();

	ClientPacketHandler::HandlePacket(session, buffer, len);
}

void GameSession::OnSend(int32 len)
{

}
