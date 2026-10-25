#include "pch.h"
#include "GameScene.h"
#include "Zone.h"
#include "World.h"

void GameScene::StartUpdate()
{
	_lastUpdateTick = ::GetTickCount64();
	Update();
}

void GameScene::Update()
{
	const uint64 now = ::GetTickCount64();
	uint64 deltaTime = now - _lastUpdateTick;
	_lastUpdateTick = now;

	for (ZoneRef& zone : _zones)
	{
		zone->Update(_lastUpdateTick, deltaTime);
	}

	ProcessMovingEntity();

	for (ZoneRef& zone : _zones)
	{
		zone->ClearPengingJobs();
	}

	DoTimer(Config::World::WORLD_UPDATE_TICK, &GameScene::Update);
}

void GameScene::ProcessMovingEntity()
{
	for (ZoneRef& zone : _zones)
	{
		Vector<EntityRef>& movingEntityList = zone->GetMovingEntity();

		for (EntityRef& entity : movingEntityList)
		{
			ZoneRef oldZone = entity->GetZone();
			ZoneRef newZone = GWorld->GetZoneByPos(entity->GetLocation());

			MoveResultRef result = GWorld->GetMoveResult(oldZone, newZone);

			if (oldZone != newZone)
				HandleZoneChanged(entity, oldZone, newZone);

			BroadcastMoveResult(result->enterZones, entity, VISION_TYPE::SPAWN);
			BroadcastMoveResult(result->leaveZones, entity, VISION_TYPE::DESPAWN);
			BroadcastMoveResult(result->keepZones, entity, VISION_TYPE::MOVE);
		}
	}
}

void GameScene::HandleZoneChanged(EntityRef entity, ZoneRef oldZone, ZoneRef newZone)
{
	GameSceneRef oldScene = oldZone->GetScene();
	GameSceneRef newScene = newZone->GetScene();

	oldZone->LeaveZone(entity);

	if (oldScene != newScene)
	{
		newScene->DoAsync([newScene, newZone, entity]()
			{
				entity->SetZone(newZone);
				entity->SetScene(newScene);
				newZone->EnterZone(entity);
			});
	}
	else
	{
		entity->SetZone(newZone);
		newZone->EnterZone(entity);
	}
}

void GameScene::BroadcastMoveResult(Vector<ZoneRef>& zones, EntityRef entity, VISION_TYPE type)
{
	if (zones.size() == 0)
		return;

	auto fn = [type](Vector<ZoneRef> zones, EntityRef entity) {

		switch (type)
		{
		case VISION_TYPE::SPAWN:
		{
			for (const ZoneRef zone : zones)
				zone->AddPendingSpawn(entity);
		}
		break;
		case VISION_TYPE::DESPAWN:
		{
			for (const ZoneRef zone : zones)
				zone->AddPendingDespawn(entity->GetObjectId());
		}
		break;
		case VISION_TYPE::MOVE:
		{
			for (const ZoneRef zone : zones)
				zone->AddPendingMove(entity);
		}
		break;
		}
		};

	HashMap<GameScene*, Vector<ZoneRef>> list;
	for (const ZoneRef& zone : zones)
		list[zone->GetScene().get()].push_back(zone);

	for (auto& [scene, zones] : list)
	{
		if (scene == this)
		{
			fn(zones, entity);
		}
		else
		{
			scene->DoAsync([fn, zones, entity]() { fn(zones, entity); });
		}
	}
}