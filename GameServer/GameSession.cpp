#include "pch.h"
#include "GameSession.h"
#include "SessionManager.h"

#include "ClientPacketHandler.h"
#include "DBManager.h"
#include "player.h"
#include "job.h"
#include "World.h"

#pragma optimize("", off)
GameSession::~GameSession()
{
	LOG_INFO(L"Delete Session");
}
#pragma optimize("", on)

///////////////////////////////////////////////////////
//	<	GameSession	>
///////////////////////////////////////////////////////
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

	if (ClientPacketHandler::HandlePacket(session, buffer, len) == false)
	{
		Disconnect();
	}
}

void GameSession::LeavePlayer()
{
	ReleaseEnterGame();

	PlayerRef player = GetPlayer();
	if (player == nullptr)
		return;

	SetPlayer(nullptr);

	player->SetDirtyFlag(PLAYER_DIRTY_ALL);
	player->UpdateDB();

	GWorld->DoAsync(&World::LeaveEntity, static_pointer_cast<Entity>(player));
}
