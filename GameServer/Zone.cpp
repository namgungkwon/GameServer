#include "pch.h"
#include "Zone.h"

#include "entity.h"
#include "player.h"
#include "Monster.h"
#include "ClientPacketHandler.h"

#include "MovementSystem.h"
#include "CombatSystem.h"
#include "AISystem.h"

void Zone::Update(uint64 now, uint64 deltaTime)
{
	_movingEntity.clear();
	MovementSystem::Update(shared_from_this(), now, deltaTime, OUT _movingEntity);
	AISystem::Update(shared_from_this(), now, deltaTime, OUT _movingEntity);
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
	case Protocol::OBJECT_CATEGORY_MONSTER:
	{
		MonsterRef monster = static_pointer_cast<Monster>(entity);
		_monsters[objectId] = monster;
		break;
	}
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
	case Protocol::OBJECT_CATEGORY_MONSTER:
	{
		_monsters.erase(objectId);
		break;
	}
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

void Zone::DespawnPtkAboutLeavePlayer(PlayerRef player)
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

EntityRef Zone::FindEntity(uint64 objectId)
{
	if (_players.find(objectId) != _players.end())
		return _players[objectId];

	if (_monsters.find(objectId) != _monsters.end())
		return _monsters[objectId];
}

void Zone::GetAdjacentZoneInfo(OUT HashMap<GameScene*, Vector<ZoneRef>>& info)
{
	Vector<ZoneRef> adjacentZone = GetAdjacentZones();

	info.clear();
	for (const ZoneRef& zone : adjacentZone)
		info[zone->GetScene().get()].push_back(zone);
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
