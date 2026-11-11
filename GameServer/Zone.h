#pragma once
#include "Protocol.pb.h"
#include "ClientPacketHandler.h"

class Zone : public enable_shared_from_this<Zone>
{
public:
	Zone(JobQueueRef sender) : _sender(sender) {};

public:
	void Update(uint64 now, uint64 deltaTime);
	void ClearPengingJobs();

public:
	void EnterZone(EntityRef entity);
	void LeaveZone(EntityRef entity);
	void SpawnPktToEnterPlayer(PlayerRef player);
	void DespawnPktToLeavePlayer(PlayerRef player);

public:
	HashMap<uint64, PlayerRef> GetPlayers() { return _players; }
	HashMap<uint64, MonsterRef> GetMonsters() { return _monsters; }
	EntityRef FindEntity(uint64 objectId);
	Vector<EntityRef>& GetMovingEntity() { return _movingEntity; }
	void GetAdjacentZoneInfo(OUT HashMap<GameScene*, Vector<ZoneRef>>& info);

public:
	void AddPendingSpawn(EntityRef entity) { _pendingSpawns.push_back(entity); }
	void AddPendingDespawn(uint64 objectId) { _pendingDespawns.push_back(objectId); }
	void AddPendingMove(EntityRef entity) { _pendingMoves.push_back(entity); }

public:
	template<typename T>
	void PacketSend(T& pkt, PlayerRef& player)
	{
		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
		BufferSend(sendBuffer, player);
	}
	void BufferSend(const SendBufferRef& sendBuffer, PlayerRef& player);

	template<typename T>
	void PacketBroadCast(T& pkt, uint64 expectId = -1)
	{
		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
		BufferBroadCast(sendBuffer);
	}

	void BufferBroadCast(const SendBufferRef& sendBuffer, uint64 expectId = -1);

private:
	GETSET(int32,					_zoneId,		ZoneId);
	GETSET_WEAK(GameScene,			_ownerScene,	Scene);
	GETSET_REF(Vector<ZoneRef>,		_adjacentZones, AdjacentZones);

private:
	HashMap<uint64, PlayerRef> _players;
	HashMap<uint64, MonsterRef> _monsters;

private:
	Vector<EntityRef> _pendingSpawns;
	Vector<uint64> _pendingDespawns;
	Vector<EntityRef> _pendingMoves;

private:
	Vector<EntityRef> _movingEntity;

public:
	JobQueueRef _sender;
};