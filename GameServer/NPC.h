#pragma once
#include "Entity.h"


///////////////////////////////////////////////////////
//	<	NPC	>
///////////////////////////////////////////////////////
class NPC : public Entity
{
public:
	virtual void				Update(uint64 nowMs, uint64 deltaTime) = 0;
	void						ResetPath();

protected:
	virtual void				UpdateIdle(uint64 nowMs, uint64 deltaTime) {};
	virtual void				UpdatePatrol(uint64 nowMs, uint64 deltaTime) {};
	virtual void				UpdateChase(uint64 nowMs, uint64 deltaTime) {};
	virtual void				UpdateAttack(uint64 nowMs, uint64 deltaTime) {};
	virtual void				UpdateDead(uint64 nowMs, uint64 deltaTime) {};

protected:
	Vector3						GetRandomLocationInPatrol();

private:
	GETSET(Vector3,				_spawnLocation,				SpawnLocation);
	GETSET(float,				_patrolRadius,				PatrolRadius);

protected:
	uint64						_nextPathFindTick			= 0;

protected:
	int64						_leftTargetGahterSceneCount	= 0;

private:
	/* path */
	GET_REF(Vector<Vector3>,	_path,						Path);
	GETSET(int32,				_pathIndex,					PathIndex);
};