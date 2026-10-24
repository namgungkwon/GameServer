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
    _playerData.clear();

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

PlayerData* GameData::GetPlayerData(int32 level)
{
    auto it = _playerData.find(level);
    return it != _playerData.end() ? &it->second : nullptr;
}

SkillData* GameData::GetSkillData(int32 skillId, int32 skillLevel)
{
    auto it = _skillData.find({ skillId, skillLevel });
    return it != _skillData.end() ? &it->second : nullptr;
}
