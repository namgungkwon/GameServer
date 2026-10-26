#pragma once
#include "Entity.h"

class AIComponent final : public IObjectComponent
{
public:
	AIComponent(EntityRef& owner) : IObjectComponent(owner) {};
public:
	void Init(Vector3 spawnLocation, float patrolRaduis, float searchRange, float maxSearchRange, float attackRange, uint64 attackDelayMs, uint64 respawnMs)
	{
		_spawnLocation = spawnLocation;
		_patrolRadius = 
		_searchRange = searchRange;
		_maxSearchRange = maxSearchRange;
		_attackRange = attackRange;
		_attackDelayMs = attackDelayMs;
	}

	bool CanAttack(uint64 nowMs) { return nowMs > _nextAttackTick; }
	void Attack(uint64 nowMs) { _nextAttackTick = nowMs + _attackDelayMs; }

	bool CanRespawn(uint64 nowMs) { return nowMs > _nextRespawnTick; }
	void Respawn(uint64 nowMs);

public:
	void Update(uint64 nowMs, uint64 deltaTime);

private:
	virtual void UpdateIdle(uint64 nowMs, uint64 deltaTime);
	virtual void UpdateChase(uint64 nowMs, uint64 deltaTime);
	virtual void UpdateAttack(uint64 nowMs, uint64 deltaTime);
	virtual void UpdateDead(uint64 nowMs, uint64 deltaTime);

private:
	Vector3 GetRandomLocationInPatrol();

private:
	GET(Vector3, _spawnLocation, SpawnLocation);
	GET(float, _patrolRadius, PatrolRadius);
	GET(float, _searchRange, SearchRange);
	GET(float, _maxSearchRange, MaxSearchRange);
	GET(float, _attackRange, AttackRange);
	uint64  _attackDelayMs;
	uint64 _respawnMs;

private:
	uint64 _nextAttackTick = 0;
	uint64 _nextRespawnTick = 0;
	uint64 _nextPathFindTick = 0;
	uint64 _nextPlayerFindTick = 0;

private:
	EntityRef _entity;
	EntityRef _target = nullptr;
	uint64 _leftTargetGahterSceneCount = 0;

private:
	/* path */
	Vector<Vector3> _path;
	uint64 _pathIndex;
	Vector3 _lastTargetLocation;
};