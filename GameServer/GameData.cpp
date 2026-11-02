#include "pch.h"
#include "GameData.h"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

bool LoadJsonArray(const char* path, json& outArray)
{
    outArray = json();

    std::ifstream file(path);
    if (!file.is_open())
    {
        LOG_ERROR(Utils::ToWString(path) + L" open failed");
        return false;
    }

    json j = json::parse(file, nullptr, false);
    if (j.is_discarded() || !j.contains("data"))
    {
        LOG_ERROR(Utils::ToWString(path) + L" parse failed");
        return false;
    }

    outArray = std::move(j["data"]);
    return true;
}


bool GameData::LoadData()
{
	if (LoadPlayerData() == false) return false;
    if (LoadMonsterSpawnData() == false) return false;
    if (LoadMonsterData() == false) return false;
    if (LoadSkillData() == false) return false;
    if (LoadSpawnPointData() == false) return false;

	return true;
}

bool GameData::LoadPlayerData()
{
    _playerData.clear();

    json data;
    if (!LoadJsonArray("../GameData/PlayerData.json", data))
        return false;

    for (const auto& item : data)
    {
        PlayerData data;
        data.id = item["id"];
        data.level = item["level"];
        data.maxHp = item["maxHp"];
        data.maxMp = item["maxMp"];
        data.requireExp = item["requireExp"];
        data.attack = item["attack"];
        data.defense = item["defense"];
        data.speed = item["speed"];

        _playerData[data.level] = std::move(data);
    }
    return true;
}

bool GameData::LoadSkillData()
{
    _skillData.clear();

    json data;
    if (!LoadJsonArray("../GameData/SkillData.json", data))
        return false;

    for (const auto& item : data)
    {
        SkillData data;
        data.id = item["id"];
        data.level = item["level"];
        data.attackRate = item["attackRate"];
        data.attackRange = item["attackRange"];
        data.attackAngle = item["attackAngle"];
        data.cooldownMs = item["cooldownMs"];
        data.moveDelayMs = item["moveDelayMs"];

        _skillData[{data.id, data.level}] = std::move(data);
    }
    return true;
}

bool GameData::LoadMonsterSpawnData()
{
    _monsterSpawnData.clear();

    json data;
    if (!LoadJsonArray("../GameData/SpawnData.json", data))
        return false;
    
    for (const auto& item : data)
    {
        MonsterSpawnData data;
        data.monsterId = item["monsterId"];
        data.location.x = item["xLocation"];
        data.location.y = item["yLocation"];
        data.respawnMs = item["respawnMs"];
        data.spawnCount = item["spawnCount"];
        data.patrolRadius = item["patrolRadius"];

        _monsterSpawnData.push_back(std::move(data));
    }
    return true;
}

bool GameData::LoadMonsterData()
{
    _monsterData.clear();

    json data;
    if (!LoadJsonArray("../GameData/MonsterData.json", data))
        return false;

    for (const auto& item : data)
    {
        MonsterData data;
        data.id = item["id"];
        data.level = item["level"];
        data.name = Utils::ToWString(item["name"].get<string>());
        data.maxHp = item["maxHp"];
        data.attack = item["attack"];
        data.defense = item["defense"];
        data.rewardExp = item["rewardExp"];
        data.speed = item["speed"];
        data.searchRange = item["searchRange"];
        data.maxSearchRange = item["maxSearchRange"];
        data.attackRange = item["attackRange"];
        data.attackDelayMs = item["attackDelayMs"];

        _monsterData[data.id] = std::move(data);
    }
    return true;
}

bool GameData::LoadSpawnPointData()
{
    _spawnPointData.clear();

    json data;
    if (!LoadJsonArray("../GameData/RespawnPoint.json", data))
        return false;

    for (const auto& item : data)
    {
        SpawnPointData data;
        data.spawnLocation.x = item["xLocation"];
        data.spawnLocation.y = item["yLocation"];
        data.spawnLocation.z = 0.f;

        _spawnPointData.push_back(std::move(data));
    }
    return true;
}

PlayerData* GameData::GetPlayerData(int32 level)
{
    auto it = _playerData.find(level);
    return it != _playerData.end() ? &it->second : nullptr;
}

SkillData* GameData::GetSkillData(uint64 skillId, int32 skillLevel)
{
    auto it = _skillData.find({ skillId, skillLevel });
    return it != _skillData.end() ? &it->second : nullptr;
}

MonsterData* GameData::GetMonsterData(uint64 monsterId)
{
    auto it = _monsterData.find(monsterId);
    return it != _monsterData.end() ? &it->second : nullptr;
}

SpawnPointData* GameData::GetSpawnPoint(const Vector3& currentLocation)
{
    SpawnPointData* ret = nullptr;
    float minDistanceSq = 99999999999999999;

    for (auto& data : _spawnPointData)
    {
        float distSq = currentLocation.DistanceSq(data.spawnLocation);

        if (distSq < minDistanceSq)
        {
            minDistanceSq = distSq;
            ret = &data;
        }
    }

    return ret;
}
