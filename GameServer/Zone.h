#pragma once
class Zone
{
public:
	Zone() = default;

public:
	void EnterZone(EntityRef entity);
	void LeaveZone(EntityRef entity);

	void AddPendingSpawn(EntityRef entity) { _pendingSpawns.push_back(entity); }
	void AddPendingDespawn(EntityRef entity) { _pendingDespawns.push_back(entity); }
	void AddPendingMove(EntityRef entity) { _pendingMoves.push_back(entity); }

private:
	GETSET(int32, _zoneId, ZoneId);
	GETSET_WEAK(GameScene, _ownerScene, Scene);
	GETSET_REF(Vector<ZoneRef>, _adjacentZones, AdjacentZones);

private:
	HashMap<uint64, PlayerRef> _players;

private:
	Vector<EntityRef> _pendingSpawns;
	Vector<EntityRef> _pendingDespawns;
	Vector<EntityRef> _pendingMoves;
};