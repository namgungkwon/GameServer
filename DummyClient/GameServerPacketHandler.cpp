#include "pch.h"
#include "GameServerPacketHandler.h"
#include "LoginServerPacketHandler.h"

#include "GameServerSession.h"
#include "LoginServerSession.h"
#include "DummyClient.h"

bool Handle_GC_CHAT(PacketSessionRef& session, Protocol::GC_CHAT& pkt)
{
	// 처리 X
	return true;
}

bool Handle_GC_AUTH_TOKEN(PacketSessionRef& session, Protocol::GC_AUTH_TOKEN& pkt)
{
	GameServerSessionRef gameSession = static_pointer_cast<GameServerSession>(session);
	if (gameSession == nullptr)
		return true;

	DummyClientRef dummy = gameSession->GetOwner();
	if (dummy == nullptr)
		return true;

	Protocol::Result result = pkt.result();

	if (result != Protocol::SUCCESS)
	{
		dummy->DisconnectGameServer();
		return true;
	}

	if (pkt.summaries().size() == 0)
	{
		dummy->RequestCreateCharacter();
	}

	for (auto& item : pkt.summaries())
	{
		dummy->RequestEnterGame(item.objectid());
	}


	return true;
}

bool Handle_GC_CREATE_CHARACTER(PacketSessionRef& session, Protocol::GC_CREATE_CHARACTER& pkt)
{
	GameServerSessionRef gameSession = static_pointer_cast<GameServerSession>(session);
	if (gameSession == nullptr)
		return true;

	DummyClientRef dummy = gameSession->GetOwner();
	if (dummy == nullptr)
		return true;

	Protocol::Result result = pkt.result();

	if (result != Protocol::SUCCESS)
	{
		dummy->DisconnectGameServer();
		return true;
	}

	for (auto& item : pkt.summaries())
	{
		dummy->RequestEnterGame(item.objectid());
	}

	return true;
}

bool Handle_GC_ENTER_GAME(PacketSessionRef& session, Protocol::GC_ENTER_GAME& pkt)
{
	GameServerSessionRef gameSession = static_pointer_cast<GameServerSession>(session);
	if (gameSession == nullptr)
		return true;

	DummyClientRef dummy = gameSession->GetOwner();
	if (dummy == nullptr)
		return true;

	bool result = pkt.success();

	if (result == false)
	{
		dummy->DisconnectGameServer();
		return true;
	}

	Protocol::ObjectData data = pkt.object();

	dummy->posData = *data.mutable_pos();
	dummy->statData = *data.mutable_stat();
	dummy->summaryData = *data.mutable_summary();
	
	dummy->EnterGame();

	return true;
}

bool Handle_GC_SPAWN(PacketSessionRef& session, Protocol::GC_SPAWN& pkt)
{
	// 처리 X
	return true;
}

bool Handle_GC_DESPAWN(PacketSessionRef& session, Protocol::GC_DESPAWN& pkt)
{
	// 처리 X
	return true;
}

bool Handle_GC_REVIVE(PacketSessionRef& session, Protocol::GC_REVIVE& pkt)
{
	return false;
}

bool Handle_GC_MOVE(PacketSessionRef& session, Protocol::GC_MOVE& pkt)
{
	GameServerSessionRef gameSession = static_pointer_cast<GameServerSession>(session);
	if (gameSession == nullptr)
		return true;

	DummyClientRef dummy = gameSession->GetOwner();
	if (dummy == nullptr)
		return true;

	for (auto& item : pkt.movedata())
	{
		if (item.objectid() != dummy->playerId)
			continue;

		//dummy->posData = item;
	}

	return true;
}

bool Handle_GC_ATTACK(PacketSessionRef& session, Protocol::GC_ATTACK& pkt)
{
	// 처리 X
	return true;
}

bool Handle_GC_DAMAGE(PacketSessionRef& session, Protocol::GC_DAMAGE& pkt)
{
	GameServerSessionRef gameSession = static_pointer_cast<GameServerSession>(session);
	if (gameSession == nullptr)
		return true;

	DummyClientRef dummy = gameSession->GetOwner();
	if (dummy == nullptr)
		return true;

	if (pkt.objectid() != dummy->playerId)
		return true;

	// TODO

	return true;
}

bool Handle_GC_REWARD(PacketSessionRef& session, Protocol::GC_REWARD& pkt)
{
	// 처리 X
	return true;
}

bool Handle_GC_LEVELUP(PacketSessionRef& session, Protocol::GC_LEVELUP& pkt)
{
	// 처리 X
	return true;
}
