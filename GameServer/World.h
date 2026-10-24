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

public:
	void EnterPlayer(GameSessionRef session, PlayerSummaryData summary, PlayerLoadData load);
	void EnterEntity(EntityRef entity);

public:
	ZoneRef         GetZoneByPos(Vector3 pos);
	ZoneRef         GetZoneById(int32 id);

private:
	void CreateScene();
	void CreateZone();
	void CreateMoveTable();
	
	MoveResult		CalculateMoveResult(ZoneRef oldZone, ZoneRef newZone);

private:
	WorldConfig _config;


private:
	Vector<GameSceneRef> _scenes;
	vector<ZoneRef> _zones;

	HashMap<GameScene*, Vector<ZoneRef>> _enterSceneGroups;
	HashMap<GameScene*, Vector<ZoneRef>> _leaveSceneGroups;
	HashMap<int32, HashMap<int32, MoveResult>> _moveTable;
};