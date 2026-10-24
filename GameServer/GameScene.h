#pragma once

#include "jobQueue.h"

class GameScene : public JobQueue
{
public:
	void AddZone(ZoneRef zone) { _zones.push_back(zone); }


private:
	GETSET(int32, _id, Id);
	Vector<ZoneRef> _zones;
};

