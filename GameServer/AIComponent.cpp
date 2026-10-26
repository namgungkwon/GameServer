#include "pch.h"
#include "AIComponent.h"
#include "Entity.h"
#include "Zone.h"
#include "GameScene.h"
#include "GridMap.h"

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
	case EntityState::Attack:
		UpdateAttack(nowMs, deltaTime);
		break;
	case EntityState::Dead:
		UpdateDead(nowMs, deltaTime);
		break;
	}

	_entity = nullptr;
}

void AIComponent::UpdateIdle(uint64 nowMs, uint64 deltaTime)
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


	auto fn = [](EntityRef entity, Vector3 entityLocation, EntityRef target) 
		{
			AIComponent* ai = entity->GetComponent<AIComponent>();

			ai->_leftTargetGahterSceneCount--;

			do {
				if (target == nullptr)
					break;

				if (ai->_target == nullptr)
				{
					ai->_target = target;
					break;
				}

				float currentDistSq = entityLocation.DistanceSq(ai->_target->GetLocation());
				float newDistSq = entityLocation.DistanceSq(target->GetLocation());

				if (newDistSq < currentDistSq)
					ai->_target = target;

			} while (false);
			
			if (ai->_leftTargetGahterSceneCount == 0)
			{
				float searchRangeSq = ai->_maxSearchRange * ai->_maxSearchRange;
				if (ai->_target || ai->_spawnLocation.DistanceSq(ai->_target->GetLocation()) < searchRangeSq)
				{
					entity->SetState(EntityState::Chase);
				}
			}
		};


	HashMap<GameScene*, Vector<ZoneRef>> sceneGroups;
	for (const ZoneRef& zone : mainZone->GetAdjacentZones())
	{
		GameSceneRef scene = zone->GetScene();
		if (scene)
			sceneGroups[scene.get()].push_back(zone);
	}
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

void AIComponent::UpdateChase(uint64 nowMs, uint64 deltaTime)
{
	Vector3 location = _entity->GetLocation();
	Vector3 targetLocation = _target->GetLocation();

	bool isReturn = false;

	do {
		if (_target->GetState() == EntityState::Dead)
		{
			isReturn = true;
			break;
		}

		if (_path.size() == 0 || (_nextPathFindTick < nowMs && _lastTargetLocation.DistanceSq(targetLocation) > 10.f))
		{
			bool isFindPath = GGridMap->FindPath(location, targetLocation, _path);
			_nextPathFindTick = Config::AI::PATH_FIND_TICK;

			if (isFindPath == false)
			{
				isReturn = true;
				break;
			}
		}
	} while (false)

	

	if (isReturn)
	{

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
