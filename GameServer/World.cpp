#include "pch.h"
#include <algorithm>
#include "World.h"

#include "GameData.h"
#include "GameScene.h"
#include "Zone.h"
#include "player.h"
#include "Monster.h"
#include "GameSession.h"
#include "Protocol.pb.h"
#include "ClientPacketHandler.h"

void World::Init(const WorldConfig& config)
{
	_config = config;
	CreateScene();
	CreateZone();
	CreateMoveTable();
}

void World::Start()
{
    for (GameSceneRef& scene : _scenes)
        scene->StartUpdate();

    SpawnMonster();
}

void World::EnterPlayer(GameSessionRef session, PlayerSummaryData summary, PlayerLoadData load)
{
    if (session == nullptr)
        return;

    PlayerRef player = MakeShared<Player>();
    if (player->Init(summary, load) == false)
    {
        Protocol::GC_ENTER_GAME pkt;
        pkt.set_success(false);
        SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
        session->Send(sendBuffer);
        session->SetAccountDBId(0);
        session->ReleaseEnterGame();
        return;
    }

    player->SetSession(session);
    session->SetPlayer(player);

    {
        Protocol::GC_ENTER_GAME pkt;
        pkt.set_success(true);
        player->MakePkt(OUT * pkt.mutable_object());

        SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
        session->Send(sendBuffer);
    }
    
    if (EnterEntity(player))
    {
        const Vector<ZoneRef>& zones = player->GetZone()->GetAdjacentZones();
        
        for (const ZoneRef& zone : zones)
        {
            zone->SpawnPktToEnterPlayer(player);
        }
    }
}

bool World::EnterEntity(EntityRef entity)
{
    if (entity == nullptr)
        return false;

    ZoneRef mainZone = GetZoneByPos(entity->GetLocation());
    if (mainZone == nullptr)
        return false;

    GameSceneRef mainScene = mainZone->GetScene();
    if (mainScene == nullptr)
        return false;

    entity->SetZone(mainZone);
    entity->SetScene(mainScene);


    Vector<ZoneRef> adjacentZone = mainZone->GetAdjacentZones();

    _enterSceneGroups.clear();
    for (const ZoneRef& zone : adjacentZone)
        _enterSceneGroups[zone->GetScene().get()].push_back(zone);

    for (auto& [scene, zones] : _enterSceneGroups)
    {
        scene->DoAsync(Job([scene, zones, entity, mainZone]() {
            for (const ZoneRef& zone : zones)
            {
                if (zone == mainZone) 
                {
                    zone->EnterZone(entity);
                    entity->SetScene(zone->GetScene());
                }
                zone->AddPendingSpawn(entity);
            }
            }));
    }
}

void World::LeaveEntity(EntityRef entity)
{
    if (entity == nullptr)
        return;

    ZoneRef mainZone = entity->GetZone();
    if (mainZone == nullptr)
        return;

    entity->SetZone(nullptr);
    entity->SetScene(nullptr);

    Vector<ZoneRef> adjacentZone = mainZone->GetAdjacentZones();

    _leaveSceneGroups.clear();
    for (const ZoneRef& zone : adjacentZone)
        _leaveSceneGroups[zone->GetScene().get()].push_back(zone);

    for (auto& [scene, zones] : _leaveSceneGroups)
    {
        scene->DoAsync(Job([scene, zones, entity, mainZone, objectId = entity->GetObjectId()]() {
            for (const ZoneRef& zone : zones)
            {
                if (zone == mainZone)
                {
                    zone->LeaveZone(entity);
                    
                }
                zone->AddPendingDespawn(objectId);
            }
            }));
    }
}

void World::CreateScene()
{
    int32 count = _config.sceneCount;
    _scenes.resize(count);
    for (int32 i = 0; i < count; ++i)
    {
        _scenes[i] = MakeShared<GameScene>();
        _scenes[i]->SetId(i);
    }
}

void World::CreateZone()
{
    int32 zoneCount = _config.mapSize / _config.zoneSize;
    int32 totalZoneCount = zoneCount * zoneCount;
    _zones.reserve(totalZoneCount);

    int32 sceneCols = 1;
    for (int32 i = 1; i * i <= _config.sceneCount; ++i)
        if (_config.sceneCount % i == 0) sceneCols = i;
    int32 sceneRows = _config.sceneCount / sceneCols;

    for (int32 y = 0; y < zoneCount; ++y)
    {
        for (int32 x = 0; x < zoneCount; ++x)
        {
            int32   linearIdx = y * zoneCount + x;
            ZoneRef zone = MakeShared<Zone>();
            zone->SetZoneId(linearIdx);
            _zones.push_back(zone);

            int32 sceneX = x * sceneCols / zoneCount;
            int32 sceneZ = y * sceneRows / zoneCount;
            int32 sceneIdx = sceneZ * sceneCols + sceneX;

            _scenes[sceneIdx]->AddZone(zone);
            zone->SetScene(_scenes[sceneIdx]);
        }
    }

    auto GetAdjacent = [&](int32 x, int32 y) -> Vector<ZoneRef>
        {
            Vector<ZoneRef> adjacent;
            adjacent.reserve(9);

            for (int32 dy = -1; dy <= 1; ++dy)
            {
                for (int32 dx = -1; dx <= 1; ++dx)
                {
                    int32 tx = x + dx;
                    int32 ty = y + dy;

                    if (tx >= 0 && tx < zoneCount && ty >= 0 && ty < zoneCount)
                        adjacent.push_back(_zones[ty * zoneCount + tx]);
                }
            }
            return std::move(adjacent);
        };

    for (int32 y = 0; y < zoneCount; ++y)
    {
        for (int32 x = 0; x < zoneCount; ++x)
        {
            Vector<ZoneRef> adjacent = GetAdjacent(x, y);
            std::sort(adjacent.begin(), adjacent.end(),
                [](const ZoneRef& a, const ZoneRef& b) { return a->GetZoneId() < b->GetZoneId(); });
            _zones[y * zoneCount + x]->SetAdjacentZones(std::move(adjacent));
        }
    }
}

void World::CreateMoveTable()
{
    for (const ZoneRef& oldZone : _zones)
    {
        int32 oldId = oldZone->GetZoneId();
        for (const ZoneRef& newZone : oldZone->GetAdjacentZones())
        {
            int32 newId = newZone->GetZoneId();
            if (oldId == newId) continue;
            _moveTable[oldId][newId] = CalculateMoveResult(oldZone, newZone);
        }
    }
}

void World::SpawnMonster()
{
    Vector<SpawnData>& spawnDataList = GGameData->GetSpawnData();

    for (const SpawnData& spawnData : spawnDataList)
    {
        MonsterData* monsterData = GGameData->GetMonsterData(spawnData.monsterId);

        if (monsterData == nullptr)
            return;

        MonsterRef monster = MakeShared<Monster>();
        monster->Init(spawnData, *monsterData);
        EnterEntity(monster);
    }
}

ZoneRef World::GetZoneByPos(Vector3 pos)
{
    int32 zoneCount = _config.mapSize / _config.zoneSize;

    int32 x = static_cast<int32>(pos.x / _config.zoneSize);
    int32 z = static_cast<int32>(pos.z / _config.zoneSize);

    if (x < 0 || x >= zoneCount || z < 0 || z >= zoneCount)
        return nullptr;

    return _zones[z * zoneCount + x];
}

ZoneRef World::GetZoneById(int32 id)
{
    if (id < 0 || id >= _zones.size())
        return nullptr;

    return _zones[id];
}

MoveResultRef World::GetMoveResult(ZoneRef oldZone, ZoneRef newZone)
{
    if (oldZone == nullptr || newZone == nullptr) return nullptr;

    int32 oldId = oldZone->GetZoneId();
    int32 newId = newZone->GetZoneId();

    auto outerIt = _moveTable.find(oldId);
    if (outerIt != _moveTable.end())
    {
        auto innerIt = outerIt->second.find(newId);
        if (innerIt != outerIt->second.end())
            return innerIt->second;
    }

    return CalculateMoveResult(oldZone, newZone);
}

MoveResultRef World::CalculateMoveResult(ZoneRef oldZone, ZoneRef newZone)
{
    const Vector<ZoneRef>& oldAdj = oldZone->GetAdjacentZones();
    const Vector<ZoneRef>& newAdj = newZone->GetAdjacentZones();

    MoveResultRef result = MakeShared<MoveResult>();
    auto itOld = oldAdj.begin();
    auto itNew = newAdj.begin();

    while (itOld != oldAdj.end() && itNew != newAdj.end())
    {
        int32 oldAdjId = (*itOld)->GetZoneId();
        int32 newAdjId = (*itNew)->GetZoneId();

        if (oldAdjId < newAdjId)
            result->leaveZones.push_back(*itOld++);
        else if (oldAdjId > newAdjId)
            result->enterZones.push_back(*itNew++);
        else
        {
            result->keepZones.push_back(*itOld);
            ++itOld; ++itNew;
        }
    }

    while (itOld != oldAdj.end()) result->leaveZones.push_back(*itOld++);
    while (itNew != newAdj.end()) result->enterZones.push_back(*itNew++);

    return result;
}
