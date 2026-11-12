#include "pch.h"
#include "NPC.h"


///////////////////////////////////////////////////////
//	<	NPC	>
///////////////////////////////////////////////////////
void NPC::ResetPath()
{
	_path.clear();
	_pathIndex = 0;
}

Vector3 NPC::GetRandomLocationInPatrol()
{
	Vector3 ret = _spawnLocation;
	const int64 patrolRadiusHalf = static_cast<int64>(_patrolRadius / 2);
	ret.x += UtilMath::GetRandom(-patrolRadiusHalf, patrolRadiusHalf);
	ret.y += UtilMath::GetRandom(-patrolRadiusHalf, patrolRadiusHalf);

	return ret;
}
