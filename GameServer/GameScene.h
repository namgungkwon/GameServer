#pragma once

#include "jobQueue.h"
#include "SceneRequest.h"
#include "job.h"

enum VISION_TYPE
{
	SPAWN,
	DESPAWN,
	MOVE,
};


///////////////////////////////////////////////////////
//	<	GameScene	>
///////////////////////////////////////////////////////
class GameScene : public JobQueue
{
public:
	void			StartUpdate();

private:
	void			Update();

private:
	void			ProcessMovingEntity();
	void			HandleZoneChanged(EntityRef entity, ZoneRef oldZone, ZoneRef newZone);

public:
	EntityRef		FindNearestPlayer(const Vector<ZoneRef>& zones, Vector3 location, float searchRange);

public:
	void			AddZone(ZoneRef zone)				{ _zones.push_back(std::move(zone)); }
	void			Request(SceneRequestRef request)	{ request->Execute(static_pointer_cast<GameScene>(shared_from_this()), _lastUpdateTick); }

	void			BroadCastAdjacentZone(const ZoneRef& zone, const SendBufferRef& sendBuffer, uint64 exceptId = -1);
	void			BroadCast(SendBufferRef sendBuffer, uint64 exceptId = -1);

private:
	void			BroadcastMoveResult(const Vector<ZoneRef>& zones, EntityRef entity, VISION_TYPE type);

private:
	GETSET(int32,	_id,		Id);
	Vector<ZoneRef> _zones;

private:
	uint64			_lastUpdateTick = 0;
};