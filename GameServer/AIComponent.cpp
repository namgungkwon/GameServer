#include "pch.h"
#include "AIComponent.h"
#include "Entity.h"
#include "Zone.h"
#include "GameScene.h"
#include "GridMap.h"

#include "MovementComponent.h"

void AIComponent::Respawn(uint64 nowMs)
{
	if (CanRespawn(nowMs) == false)
		return;

	_nextRespawnTick = nowMs + _respawnMs;

	EntityRef entity = _owner.lock();
	if (entity == nullptr)
		return;

	entity->Respawn(nowMs);
}

void AIComponent::Update(uint64 nowMs, uint64 deltaTime)
{
	_entity = _owner.lock();
	if (_entity == nullptr)
		return;

	switch (_entity->GetState())
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

	_entity = nullptr;
}

void AIComponent::ResetPath()
{
	_path.clear();
	_pathIndex = 0;
	_lastTargetLocation = Vector3();
}

void AIComponent::UpdateIdle(uint64 nowMs, uint64 deltaTime)
{
	FindPlayer(nowMs);
}

void AIComponent::UpdatePatrol(uint64 nowMs, uint64 deltaTime)
{
	if (_path.size() == 0)
	{
		Vector3 rndLocation = GetRandomLocationInPatrol();

		if (GGridMap->CanGo(rndLocation))
		{
			_path.push_back(rndLocation);
			_pathIndex = 0;
		}
	}

	FindPlayer(nowMs);
}

void AIComponent::UpdateChase(uint64 nowMs, uint64 deltaTime)
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

		Vector3 location = _entity->GetLocation();
		Vector3 targetLocation = target->GetLocation();

		float maxSearchRangeSq = _maxSearchRange * _maxSearchRange;
		if (targetLocation.DistanceSq(_spawnLocation) > maxSearchRangeSq)
		{
			LOG_INFO(L"target Out Of ChaseRange");
			break;
		}

		isReturn = false;
		// TODO 거리가 공격거리 안이면 attack으로 전환

		if (_path.size() == 0 || (_nextPathFindTick < nowMs && _lastTargetLocation.DistanceSq(targetLocation) > 10.f))
		{
			ResetPath();
			_nextPathFindTick = Config::AI::PATH_FIND_TICK + nowMs;

			if (GGridMap->FindPath(location, targetLocation, _path))
			{
				_lastTargetLocation = target->GetLocation();

				if (_path.back().DistanceSq(targetLocation) > 0.1f)
					_path.back() = targetLocation;
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
		_entity->SetState(EntityState::Idle);
		return;
	}

}

void AIComponent::UpdateAttack(uint64 nowMs, uint64 deltaTime)
{}

void AIComponent::UpdateDead(uint64 nowMs, uint64 deltaTime)
{}

Vector3 AIComponent::GetRandomLocationInPatrol()
{
	Vector3 ret = _spawnLocation;
	int64 patrolRadiusHalf = static_cast<int64>(_patrolRadius) / 2;
	ret.x += UtilMath::GetRandom(-patrolRadiusHalf, patrolRadiusHalf);
	ret.y += UtilMath::GetRandom(-patrolRadiusHalf, patrolRadiusHalf);

	return ret;
}

void AIComponent::FindPlayer(uint64 nowMs)
{
	if (nowMs < _nextPlayerFindTick)
		return;

	if (_leftTargetGahterSceneCount > 0)
		return;

	_nextPlayerFindTick = nowMs + Config::AI::PLAYER_FIND_TICK;

	ZoneRef mainZone = _entity->GetZone();
	GameScene* mainScene = _entity->GetScene().get();
	if (mainZone == nullptr || mainScene == nullptr)
		return;


	auto fn = [](EntityRef entity, Vector3 entityLocation, EntityRef newTarget)
		{
			AIComponent* ai = entity->GetComponent<AIComponent>();

			ai->_leftTargetGahterSceneCount--;

			do {
				if (newTarget == nullptr)
					break;

				EntityRef target = ai->_target.lock();
				if (target == nullptr)
				{
					ai->_target = newTarget;
					break;
				}

				float currentDistSq = entityLocation.DistanceSq(target->GetLocation());
				float newDistSq = entityLocation.DistanceSq(newTarget->GetLocation());

				if (newDistSq < currentDistSq)
					ai->_target = newTarget;

			} while (false);

			if (ai->_leftTargetGahterSceneCount == 0)
			{
				float searchRangeSq = ai->_maxSearchRange * ai->_maxSearchRange;
				EntityRef target = ai->_target.lock();
				if (target && ai->_spawnLocation.DistanceSq(target->GetLocation()) < searchRangeSq)
				{
					ai->_lastTargetLocation = target->GetLocation();
					ai->ResetPath();
					entity->SetState(EntityState::Chase);
					LOG_INFO(L"Find Target");
				}
				else
				{
					entity->SetState(EntityState::Patrol);
				}
			}
		};


	HashMap<GameScene*, Vector<ZoneRef>> sceneGroups;
	mainZone->GetAdjacentZoneInfo(OUT sceneGroups);

	_leftTargetGahterSceneCount = sceneGroups.size();
	Vector3 location = _entity->GetLocation();
	EntityRef owner = _entity;

	for (auto& [scene, zones] : sceneGroups)
	{
		if (scene == mainScene)
		{
			EntityRef nearestPlayer = mainScene->FindNearestPlayer(zones, location, _searchRange);
			fn(_entity, location, nearestPlayer);
		}
		else
		{
			GameSceneRef targetScene = zones[0]->GetScene();
			if (!targetScene) continue;
			targetScene->DoAsync([mainScene, targetScene, zones, owner, location, searchRange = _searchRange, fn]()
				{
					EntityRef nearestPlayer = targetScene->FindNearestPlayer(zones, location, searchRange);

					mainScene->DoAsync([owner, location, nearestPlayer, fn]() { fn(owner, location, nearestPlayer); });
				});
		}
	}
}
