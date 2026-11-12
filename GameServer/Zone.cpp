#include "pch.h"
#include "Zone.h"

#include "entity.h"
#include "player.h"
#include "Monster.h"
#include "ClientPacketHandler.h"
#include "PacketSender.h"

#include "MovementSystem.h"
#include "CombatSystem.h"
#include "AISystem.h"


///////////////////////////////////////////////////////
//	<	Zone	>
///////////////////////////////////////////////////////
void Zone::Update(uint64 now, uint64 deltaTime)
{
	MovementSystem::Update(shared_from_this(), now, deltaTime, OUT _movingEntity);
	AISystem::Update(shared_from_this(), now, deltaTime, OUT _movingEntity);
}

void Zone::EnterZone(EntityRef entity)
{
	if (entity == nullptr)
		return;

	const uint64 objectId = entity->GetObjectId();

	switch (entity->GetType())
	{
	case Protocol::OBJECT_CATEGORY_PLAYER:
	{
		PlayerRef player = static_pointer_cast<Player>(entity);
		_players[objectId] = std::move(player);
		break;
	}
	case Protocol::OBJECT_CATEGORY_MONSTER:
	{
		MonsterRef monster = static_pointer_cast<Monster>(entity);
		_monsters[objectId] = std::move(monster);
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

	const uint64 objectId = entity->GetObjectId();

	switch (entity->GetType())
	{
	case Protocol::OBJECT_CATEGORY_PLAYER:
	{
		_players.erase(objectId);
		if (_players.empty())
		{
			_pendingSpawns.clear();
			_pendingDespawns.clear();
			_pendingMoves.clear();
		}
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
		if (pkt.object_size() < Config::Packet::MAXCOUNT_SPAWNPKT)
			continue;
		PacketSend(pkt, player);
		pkt.Clear();
	}
	for (auto& [id, object] : _monsters)
	{
		object->MakePkt(*pkt.add_object());
		if (pkt.object_size() < Config::Packet::MAXCOUNT_SPAWNPKT)
			continue;
		PacketSend(pkt, player);
		pkt.Clear();
	}

	if (pkt.object_size() > 0)
	{
		PacketSend(pkt, player);
	}
}

void Zone::DespawnPktToLeavePlayer(PlayerRef player)
{
	Protocol::GC_DESPAWN pkt;
	for (auto& [id, object] : _players)
	{
		pkt.add_objectid(id);
		if (pkt.objectid().size() < Config::Packet::MAXCOUNT_DESPAWNPKT)
			continue;
		PacketSend(pkt, player);
		pkt.Clear();
	}
	for (auto& [id, object] : _monsters)
	{
		pkt.add_objectid(id);
		if (pkt.objectid().size() < Config::Packet::MAXCOUNT_DESPAWNPKT)
			continue;
		PacketSend(pkt, player);
	}

	if (pkt.objectid_size() > 0)
	{
		PacketSend(pkt, player);
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
	const Vector<ZoneRef>& adjacentZone = GetAdjacentZones();

	info.clear();
	for (const ZoneRef& zone : adjacentZone)
		info[zone->GetScene().get()].push_back(zone);
}

void Zone::BufferSend(SendBufferRef sendBuffer, PlayerRef player)
{
	_sender->DoAsync([target = std::move(player), sendBuffer = std::move(sendBuffer)]() {
			target->Send(sendBuffer);
		});
}

void Zone::BufferBroadCast(SendBufferRef sendBuffer, uint64 expectId)
{
	Vector<PlayerRef> targets;
	targets.reserve(_players.size());
	for (auto& [id, player] : _players)
	{
		if (expectId == id)
			continue;
		targets.push_back(player);
	}

	_sender->DoAsync([targets = std::move(targets), sendBuffer = std::move(sendBuffer)]() {
		for (const PlayerRef& player : targets)
		{
			player->Send(sendBuffer);
		}
		});
}

void Zone::ClearPengingJobs()
{
	if (_players.empty())
	{
		return;
	}

	{
		Protocol::GC_SPAWN pkt;
		for (EntityRef& entity : _pendingSpawns)
		{
			entity->MakePkt(*pkt.add_object());
			if (pkt.object_size() < Config::Packet::MAXCOUNT_SPAWNPKT)
				continue;

			PacketBroadCast(pkt);
			pkt.Clear();
		}

		if (pkt.object_size() > 0)
			PacketBroadCast(pkt);
		_pendingSpawns.clear();
	}
	{
		Protocol::GC_DESPAWN pkt;
		for (uint64 id : _pendingDespawns)
		{
			pkt.add_objectid(id);

			if (pkt.objectid().size() < Config::Packet::MAXCOUNT_DESPAWNPKT)
				continue;

			PacketBroadCast(pkt);
			pkt.Clear();
		}
		if (pkt.objectid_size() > 0) 
			PacketBroadCast(pkt);
		_pendingDespawns.clear();
	}
	{
		Protocol::GC_MOVE pkt;
		for (EntityRef entity : _pendingMoves)
		{
			entity->MakePkt(*pkt.add_movedata());

			if (pkt.movedata().size() < Config::Packet::MAXCOUNT_MOVEPKT)
				continue;

			PacketBroadCast(pkt);
			pkt.Clear();
		}
		if (pkt.movedata_size() > 0)
			PacketBroadCast(pkt);
		_pendingMoves.clear();
	}
}