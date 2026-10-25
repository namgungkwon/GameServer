#pragma once

#include "jobqueue.h"

struct MoveResult
{
	Vector<ZoneRef> leaveZones;
	Vector<ZoneRef> enterZones;
	Vector<ZoneRef> keepZones;
};

class World : public JobQueue
{
public:
	void Init(const WorldConfig& config);
	void Start();

public:
	void EnterPlayer(GameSessionRef session, PlayerSummaryData summary, PlayerLoadData load);
	bool EnterEntity(EntityRef entity);
	void LeaveEntity(EntityRef entity);

public:
	ZoneRef         GetZoneByPos(Vector3 pos);
	ZoneRef         GetZoneById(int32 id);
	MoveResultRef	GetMoveResult(ZoneRef oldZone, ZoneRef newZone);

private:
	void CreateScene();
	void CreateZone();
	void CreateMoveTable();
	
	MoveResultRef		CalculateMoveResult(ZoneRef oldZone, ZoneRef newZone);

private:
	WorldConfig _config;


private:
	Vector<GameSceneRef> _scenes;
	vector<ZoneRef> _zones;

	HashMap<GameScene*, Vector<ZoneRef>> _enterSceneGroups;
	HashMap<GameScene*, Vector<ZoneRef>> _leaveSceneGroups;
	HashMap<int32, HashMap<int32, MoveResultRef>> _moveTable;
};