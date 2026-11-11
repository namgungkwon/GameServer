#include "pch.h"
#include "GameScene.h"
#include "Zone.h"
#include "World.h"

#include "Entity.h"
#include "Player.h"

void GameScene::StartUpdate()
{
	_lastUpdateTick = ::GetTickCount64();
	Update();
}

void GameScene::Update()
{
	const uint64 now = ::GetTickCount64();
	const uint64 updateTickMs = Config::World::WORLD_UPDATE_TICK;

	if (GServerStats)
	{
		uint64 expectedTickCount = _lastUpdateTick + updateTickMs;
		if (now > expectedTickCount)
		{
			uint64 lagMs = now - expectedTickCount;
			GServerStats->game.lagTickCount.fetch_add(1);
			GServerStats->game.lagTickMs.fetch_add(lagMs);

			uint64 prevMax = GServerStats->game.lagTickMaxMs.load();
			if (lagMs > prevMax)
			{
				GServerStats->game.lagTickMaxMs.store(lagMs);
			}
		}

		GServerStats->game.tickCount.fetch_add(1);
	}

	uint64 deltaTime = now - _lastUpdateTick;
	_lastUpdateTick = now;

	DoTimer(Config::World::WORLD_UPDATE_TICK, &GameScene::Update);

	for (ZoneRef& zone : _zones)
	{
		zone->Update(_lastUpdateTick, deltaTime);
	}

	ProcessMovingEntity();

	for (ZoneRef& zone : _zones)
	{
		zone->ClearPengingJobs();
	}
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
			if (result == nullptr)
				continue;

			if (oldZone != newZone)
				HandleZoneChanged(entity, oldZone, newZone);

			BroadcastMoveResult(result->enterZones, entity, VISION_TYPE::SPAWN);
			BroadcastMoveResult(result->leaveZones, entity, VISION_TYPE::DESPAWN);
			BroadcastMoveResult(result->keepZones, entity, VISION_TYPE::MOVE);
		}

		movingEntityList.clear();
	}
}

void GameScene::HandleZoneChanged(EntityRef entity, ZoneRef oldZone, ZoneRef newZone)
{
	if (oldZone == nullptr || newZone == nullptr)
		return;

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

EntityRef GameScene::FindNearestPlayer(Vector<ZoneRef> zones, Vector3 location, float searchRange)
{
	float searchRangeSq = searchRange * searchRange;
	EntityRef target = nullptr;
	float minRangeSq = 1000000000.f;
	for (ZoneRef& zone : zones)
	{
		for (auto& [id, player] : zone->GetPlayers())
		{
			if (player->GetState() == EntityState::Dead)
				continue;

			float rangeSq = location.DistanceSq(player->GetLocation());
			if (rangeSq < minRangeSq)
			{
				minRangeSq = rangeSq;
				target = player;
			}
		}
	}

	return minRangeSq < searchRangeSq ? target : nullptr;
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
			{
				zone->AddPendingSpawn(entity);

				if (entity->GetType() == Protocol::OBJECT_CATEGORY_PLAYER)
				{
					zone->SpawnPktToEnterPlayer(static_pointer_cast<Player>(entity));
				}
			}
		}
		break;
		case VISION_TYPE::DESPAWN:
		{
			for (const ZoneRef zone : zones)
			{
				zone->AddPendingDespawn(entity->GetObjectId());

				if (entity->GetType() == Protocol::OBJECT_CATEGORY_PLAYER)
				{
					zone->DespawnPktToLeavePlayer(static_pointer_cast<Player>(entity));
				}
			}
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

void GameScene::BroadCastAdjacentZone(ZoneRef zone, SendBufferRef sendBuffer, uint64 exceptId)
{
	HashMap<GameScene*, Vector<ZoneRef>> adjacentZoneInfo;
	zone->GetAdjacentZoneInfo(OUT adjacentZoneInfo);


	for (auto& [scene, zones] : adjacentZoneInfo)
	{
		if (scene == this)
		{
			for (ZoneRef& zone : zones)
				zone->BufferBroadCast(sendBuffer, exceptId);
		}
		else
		{
			scene->DoAsync([scene, zones, sendBuffer, exceptId]() {
				for (const ZoneRef& zone : zones)
					zone->BufferBroadCast(sendBuffer, exceptId);
				});
		}
	}
}

void GameScene::BroadCast(SendBufferRef sendBuffer, uint64 exceptId)
{
	for (auto& zone : _zones)
	{
		zone->BufferBroadCast(sendBuffer, exceptId);
	}
}
