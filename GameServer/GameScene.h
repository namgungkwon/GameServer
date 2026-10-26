#pragma once

#include "jobQueue.h"
#include "job.h"

enum VISION_TYPE
{
	SPAWN,
	DESPAWN,
	MOVE,
};

class GameScene : public JobQueue
{
public:
	void StartUpdate();

private:
	void Update();

private:
	void ProcessMovingEntity();
	void HandleZoneChanged(EntityRef entity, ZoneRef oldZone, ZoneRef newZone);

public:
	EntityRef FindNearestPlayer(Vector<ZoneRef> zones, Vector3 location, float searchRange);

public:
	void AddZone(ZoneRef zone) { _zones.push_back(zone); }

private:
	void BroadcastMoveResult(Vector<ZoneRef>& zones, EntityRef entity, VISION_TYPE type);

private:
	GETSET(int32, _id, Id);
	Vector<ZoneRef> _zones;

private:
	uint64 _lastUpdateTick = 0;
};

