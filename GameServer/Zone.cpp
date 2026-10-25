#include "pch.h"
#include "Zone.h"

#include "entity.h"
#include "player.h"
#include "ClientPacketHandler.h"

#include "MovementSystem.h"

void Zone::Update(uint64 now, uint64 deltaTime)
{
	_movingEntity.clear();
	MovementSystem::Update(shared_from_this(), now, deltaTime, OUT _movingEntity);
}

void Zone::EnterZone(EntityRef entity)
{
	if (entity == nullptr)
		return;

	uint64 objectId = entity->GetObjectId();

	switch (entity->GetType())
	{
	case Protocol::OBJECT_CATEGORY_PLAYER:
	{
		PlayerRef player = static_pointer_cast<Player>(entity);
		_players[objectId] = player;
		break;
	}
	// Todo 
	// 타입 추가
	default:
		break;
	}
}

void Zone::LeaveZone(EntityRef entity)
{
	if (entity == nullptr)
		return;

	uint64 objectId = entity->GetObjectId();

	switch (entity->GetType())
	{
	case Protocol::OBJECT_CATEGORY_PLAYER:
	{
		_players.erase(objectId);
		break;
	}
	// Todo 
	// 타입 추가
	default:
		break;
	}
}

void Zone::SpawnPktToEnterPlayer(PlayerRef player)
{
	Protocol::GC_SPAWN pkt;
	for (auto& [id, object] : _players)
	{
		object->MakePkt(*pkt.add_object());
	}
	for (auto& [id, object] : _monsters)
	{
		object->MakePkt(*pkt.add_object());
	}

	if (pkt.object_size() > 0)
	{
		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
		player->Send(sendBuffer);
	}
}

void Zone::DespawnPtkToLeavePlayer(PlayerRef player)
{
	Protocol::GC_DESPAWN pkt;
	for (auto& [id, object] : _players)
	{
		pkt.add_objectid(id);
	}
	for (auto& [id, object] : _monsters)
	{
		pkt.add_objectid(id);
	}

	if (pkt.objectid_size() > 0)
	{
		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
		player->Send(sendBuffer);
	}
}

void Zone::ClearPengingJobs()
{
	{
		Protocol::GC_SPAWN pkt;
		for (EntityRef& entity : _pendingSpawns)
		{
			entity->MakePkt(*pkt.add_object());
		}
		if (pkt.object_size() > 0)
			BroadCast(pkt);
		_pendingSpawns.clear();
	}
	{
		Protocol::GC_DESPAWN pkt;
		for (uint64 id : _pendingDespawns)
		{
			pkt.add_objectid(id);
		}
		if (pkt.objectid_size() > 0)
			BroadCast(pkt);
		_pendingDespawns.clear();
	}
	{
		Protocol::GC_MOVE pkt;
		for (EntityRef entity : _pendingMoves)
		{
			entity->MakePkt(*pkt.add_movedata());
		}
		if (pkt.movedata_size() > 0)
			BroadCast(pkt);
		_pendingMoves.clear();
	}
}
