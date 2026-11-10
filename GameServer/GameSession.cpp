#include "pch.h"
#include "GameSession.h"
#include "SessionManager.h"

#include "ClientPacketHandler.h"
#include "DBManager.h"
#include "player.h"
#include "job.h"
#include "World.h"

void GameSession::OnConnected()
{
	PacketSession::OnConnected();
	// temp
	GSessionManager->Add(static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnDisconnected()
{
	LeavePlayer();
	GSessionManager->Remove(static_pointer_cast<GameSession>(shared_from_this()));
	PacketSession::OnDisconnected();
}

void GameSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef session = GetPacketSessionRef();
	ClientPacketHandler::HandlePacket(session, buffer, len);
}

void GameSession::LeavePlayer()
{
	ReleaseEnterGame();

	PlayerRef player = GetPlayer();
	if (player == nullptr)
		return;

	SetPlayer(nullptr);


	GDBManager->DoAsync(Job([id = player->GetDBId(),data = player->MakeLoadData()]() {
		GDBManager->SavePlayer(id, &data);
		}));

	GWorld->DoAsync(&World::LeaveEntity, static_pointer_cast<Entity>(player));
}
