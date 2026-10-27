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
		_patrolRadius = patrolRaduis;
		_searchRange = searchRange;
		_maxSearchRange = maxSearchRange;
		_attackRange = attackRange;
		_attackDelayMs = attackDelayMs;

		_path.clear();
		_pathIndex = -1;
		_lastTargetLocation = Vector3();
	}

	bool CanAttack(uint64 nowMs) { return nowMs > _nextAttackTick; }
	void Attack(uint64 nowMs) { _nextAttackTick = nowMs + _attackDelayMs; }

	bool CanRespawn(uint64 nowMs) { return nowMs > _nextRespawnTick; }
	void Respawn(uint64 nowMs);

public:
	void Update(uint64 nowMs, uint64 deltaTime);
	void ResetPath();

private:
	virtual void UpdateIdle(uint64 nowMs, uint64 deltaTime);
	virtual void UpdatePatrol(uint64 nowMs, uint64 deltaTime);
	virtual void UpdateChase(uint64 nowMs, uint64 deltaTime);
	virtual void UpdateAttack(uint64 nowMs, uint64 deltaTime);
	virtual void UpdateDead(uint64 nowMs, uint64 deltaTime);

private:
	Vector3 GetRandomLocationInPatrol();
	void MoveTick(Vector3 targetPos);
	void FindPlayer(uint64 nowMs);

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
	std::weak_ptr<Entity> _target;
	int64 _leftTargetGahterSceneCount = 0;

private:
	/* path */
	GET_REF(Vector<Vector3>, _path, Path);
	GETSET(int32, _pathIndex, PathIndex);
	Vector3 _lastTargetLocation;
};