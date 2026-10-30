#include "pch.h"
#include "Monster.h"

#include "StatComponent.h"
#include "SkillComponent.h"
#include "CombatComponent.h"
#include "MovementComponent.h"
#include "Zone.h"
#include "GameScene.h"
#include "GridMap.h"
#include "World.h"

void Monster::Init(const SpawnData& spawnData, MonsterData& data)
{
	//Entity
	SetObjectId(IdGenerator::Get());
	SetName(data.name);
	SetLevel(data.level);
	SetLocation(spawnData.location);
	SetYaw(0);
	SetType(Protocol::OBJECT_CATEGORY_MONSTER);
	SetState(EntityState::Idle);
	SetTemplateId(data.id);
	SetExp(data.rewardExp);

	// NPC
	SetSpawnLocation(spawnData.location);
	SetPatrolRadius(spawnData.patrolRadius);

	// Monster
	_searchRange = data.searchRange;
	_maxSearchRange = data.maxSearchRange;
	_attackRange = data.attackRange;
	_attackDelayMs = data.attackDelayMs;
	_respawnMs = spawnData.respawnMs;


	// Component
	AddComponent<StatComponent>(shared_from_this())->Init(data.maxHp, data.maxHp, 0, 0);
	AddComponent<CombatComponent>(shared_from_this())->Init(data.attack, data.defense);
	AddComponent<MovementComponent>(shared_from_this())->Init(data.speed);

	// temp 기본 공격은 0번 스킬
	Vector<SkillState> skills;
	skills.push_back(SkillState{ 0, 1, 0 });
	AddComponent<SkillComponent>(shared_from_this())->Init(skills);
}

void Monster::Respawn()
{
	Entity::Respawn();

	SetLocation(GetSpawnLocation());
	SetYaw(0);
	SetState(EntityState::Idle);

	StatComponent* stat = GetComponent<StatComponent>();
	stat->SetCurrentHP(stat->GetMaxHP());

	GWorld->EnterEntity(shared_from_this());
}

void Monster::Update(uint64 nowMs, uint64 deltaTime)
{
	if (GetState() != _lastState)
	{
		_lastState = GetState();

		switch (_lastState)
		{
		case EntityState::Dead:
			EnterDead(nowMs, deltaTime);
			break;
		}
	}

	switch (GetState())
	{
	case EntityState::Idle:
		UpdateIdle(nowMs, deltaTime);
		break;
	case EntityState::Chase:
		UpdateChase(nowMs, deltaTime);
		break;
	case EntityState::Patrol:
		UpdatePatrol(nowMs, deltaTime);
		break;
	case EntityState::Attack:
		UpdateAttack(nowMs, deltaTime);
		break;
	case EntityState::Dead:
		UpdateDead(nowMs, deltaTime);
		break;
	}
}

void Monster::UpdateIdle(uint64 nowMs, uint64 deltaTime)
{
	FindPlayer(nowMs);
}

void Monster::UpdatePatrol(uint64 nowMs, uint64 deltaTime)
{
	Vector<Vector3>& path = GetPath();
	if (path.size() == 0)
	{
		Vector3 rndLocation = GetRandomLocationInPatrol();

		if (GGridMap->CanGo(rndLocation))
		{
			path.push_back(rndLocation);
			SetPathIndex(0);
		}
	}

	FindPlayer(nowMs);
}

void Monster::UpdateChase(uint64 nowMs, uint64 deltaTime)
{
	bool isReturn = true;

	do {
		EntityRef target = _target.lock();
		if (target == nullptr)
		{
			break;
		}

		if (target->GetState() == EntityState::Dead)
		{
			break;
		}

		Vector3 location = GetLocation();
		Vector3 targetLocation = target->GetLocation();

		float maxSearchRangeSq = _maxSearchRange * _maxSearchRange;
		if (targetLocation.DistanceSq(GetSpawnLocation()) > maxSearchRangeSq)
		{
			LOG_INFO(L"target Out Of ChaseRange");
			break;
		}

		isReturn = false;

		float attackDistanceSq = _attackRange * _attackRange;
		if (location.DistanceSq(targetLocation) <= attackDistanceSq)
		{
			ResetPath();
			SetState(EntityState::Attack);
			break;
		}

		Vector<Vector3>& path = GetPath();
		if (path.size() == 0 || (_nextPathFindTick < nowMs && _lastTargetLocation.DistanceSq(targetLocation) > 10.f))
		{
			ResetPath();
			_nextPathFindTick = Config::AI::PATH_FIND_TICK + nowMs;

			if (GGridMap->FindPath(location, targetLocation, path))
			{
				_lastTargetLocation = target->GetLocation();

				if (path.back().DistanceSq(targetLocation) > 0.1f)
					path.back() = targetLocation;
			}
			else
			{
				isReturn = true;
			}
		}
	} while (false);

	if (isReturn)
	{
		ResetPath();
		SetState(EntityState::Idle);
		return;
	}
}

void Monster::UpdateAttack(uint64 nowMs, uint64 deltaTime)
{
	EntityRef target = _target.lock();
	if (target == nullptr)
	{
		SetState(EntityState::Idle);
		return;
	}

	Vector3 location = GetLocation();
	Vector3 targetLocation = target->GetLocation();

	float attackDistanceSq = _attackRange * _attackRange;
	if (location.DistanceSq(targetLocation) > attackDistanceSq)
	{
		SetState(EntityState::Chase);
		return;
	}

	// 공격
}

void Monster::UpdateDead(uint64 nowMs, uint64 deltaTime)
{
	//
}

void Monster::EnterDead(uint64 nowMs, uint64 deltaTime)
{
	ResetPath();
	GWorld->LeaveEntity(shared_from_this()); // Tick의 영향을 받지 않게 됨
	_nextRespawnTick = nowMs + _respawnMs;
	GWorld->DoTimer(_respawnMs, [monster = shared_from_this()]() {
		monster->Respawn();
		GWorld->EnterEntity(monster);
		});
}

void Monster::FindPlayer(uint64 nowMs)
{
	if (nowMs < _nextPlayerFindTick)
		return;

	if (_leftTargetGahterSceneCount > 0)
		return;

	_nextPlayerFindTick = nowMs + Config::AI::PLAYER_FIND_TICK;

	ZoneRef mainZone = GetZone();
	GameScene* mainScene = GetScene().get();
	if (mainZone == nullptr || mainScene == nullptr)
		return;


	auto fn = [](MonsterRef monster, Vector3 entityLocation, EntityRef newTarget)
		{
			monster->_leftTargetGahterSceneCount--;

			do {
				if (newTarget == nullptr)
					break;

				EntityRef target = monster->_target.lock();
				if (target == nullptr)
				{
					monster->_target = newTarget;
					break;
				}

				float currentDistSq = entityLocation.DistanceSq(target->GetLocation());
				float newDistSq = entityLocation.DistanceSq(newTarget->GetLocation());

				if (newDistSq < currentDistSq)
					monster->_target = newTarget;

			} while (false);

			if (monster->_leftTargetGahterSceneCount == 0)
			{
				float searchRangeSq = monster->_maxSearchRange * monster->_maxSearchRange;
				EntityRef target = monster->_target.lock();
				if (target && monster->GetSpawnLocation().DistanceSq(target->GetLocation()) < searchRangeSq)
				{
					monster->_lastTargetLocation = target->GetLocation();
					monster->ResetPath();
					monster->SetState(EntityState::Chase);
					LOG_INFO(L"Find Target");
				}
				else
				{
					monster->SetState(EntityState::Patrol);
				}
			}
		};


	HashMap<GameScene*, Vector<ZoneRef>> sceneGroups;
	mainZone->GetAdjacentZoneInfo(OUT sceneGroups);

	_leftTargetGahterSceneCount = sceneGroups.size();
	Vector3 location = GetLocation();

	for (auto& [scene, zones] : sceneGroups)
	{
		if (scene == mainScene)
		{
			EntityRef nearestPlayer = mainScene->FindNearestPlayer(zones, location, _searchRange);
			fn(static_pointer_cast<Monster>(shared_from_this()), location, nearestPlayer);
		}
		else
		{
			GameSceneRef targetScene = zones[0]->GetScene();
			if (!targetScene) continue;
			targetScene->DoAsync([mainScene, targetScene, zones, owner = static_pointer_cast<Monster>(shared_from_this()), location, searchRange = _searchRange, fn]()
				{
					EntityRef nearestPlayer = targetScene->FindNearestPlayer(zones, location, searchRange);

					mainScene->DoAsync([owner, location, nearestPlayer, fn]() { fn(owner, location, nearestPlayer); });
				});
		}
	}
}
