#pragma once

#include "MovementComponent.h"


///////////////////////////////////////////////////////
//	<	MovementSystem	>
///////////////////////////////////////////////////////
class MovementSystem
{
public:
	static void			Update(ZoneRef zone, uint64 nowTick, uint64 deltaTime, OUT Vector<EntityRef>& movingPlayer);

	static bool			UpdatePlayerMove(PlayerRef player, uint64 nowTick, uint64 deltaTime);

private:
	static Vector3		GetNextPos(MovementComponent* mc, MoveJob& job, uint64 deltaTime);
};