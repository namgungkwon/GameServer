#pragma once
#include "GameStruct.h"

template<>
struct std::hash<std::pair<int32, int32>>
{
	size_t operator()(const std::pair<int32, int32>& key) const noexcept
	{
		return std::hash<int64>{}(
			(static_cast<int64>(key.first) << 32) ^ static_cast<uint32>(key.second)
			);
	}
};


class GameData
{
public:
	bool LoadData();

private:
	bool LoadPlayerData();
	bool LoadSkillData();

public:
	PlayerData* GetPlayerData(int32 level);
	SkillData* GetSkillData(int32 skillId, int32 skillLevel);

private:
	HashMap<int32, PlayerData> _playerData; // Level, Data
	HashMap<std::pair<int32, int32>, SkillData> _skillData;
};