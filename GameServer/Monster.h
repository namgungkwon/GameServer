#pragma once

#include "NPC.h"


///////////////////////////////////////////////////////
//	<	Monster	>
///////////////////////////////////////////////////////
class Monster : public NPC
{
public:
	void					Init(const MonsterSpawnData& spawnData, MonsterData& data);
	virtual void			Respawn(const Vector3& respawnLocation) override;
	virtual void			Update(uint64 nowMs, uint64 deltaTime) override;

protected:
	virtual void			UpdateIdle(uint64 nowMs, uint64 deltaTime) override;
	virtual void			UpdatePatrol(uint64 nowMs, uint64 deltaTime) override;
	virtual void			UpdateChase(uint64 nowMs, uint64 deltaTime) override;
	virtual void			UpdateAttack(uint64 nowMs, uint64 deltaTime) override;
	virtual void			UpdateDead(uint64 nowMs, uint64 deltaTime) override;

protected:
	virtual void			EnterDead(uint64 nowMs, uint64 deltaTime);

protected:
	virtual void			FindPlayer(uint64 nowMs);

protected:
	GET(float,				_searchRange,		SearchRange);
	GET(float,				_maxSearchRange,	MaxSearchRange);
	GET(float,				_attackRange,		AttackRange);
	uint64					_attackDelayMs;
	uint64					_respawnMs;

protected:
	uint64					_nextAttackTick = 0;
	uint64					_nextRespawnTick = 0;
	uint64					_nextPlayerFindTick = 0;

protected:
	std::weak_ptr<Entity>	_target;

protected:
	Vector3					_lastTargetLocation;
	EntityState				_lastState;
};

