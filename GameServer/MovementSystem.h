#pragma once

#include "MovementComponent.h"

class MovementSystem
{
public:
	static void Update(ZoneRef zone, uint64 nowTick, uint64 deltaTime, OUT Vector<EntityRef>& movingPlayer);

	static bool UpdateEntityMove(EntityRef entity, uint64 nowTick, uint64 deltaTime);

private:
	static Vector3 GetNextPos(EntityRef entity, MoveJob& job, uint64 nowTick);
};