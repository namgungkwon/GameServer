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
	bool LoadMonsterSpawnData();
	bool LoadMonsterData();

public:
	PlayerData* GetPlayerData(int32 level);
	SkillData* GetSkillData(uint64 skillId, int32 skillLevel);
	Vector<SpawnData>& GetSpawnData() {	return _spawnData; }
	MonsterData* GetMonsterData(uint64 monsterId);

private:
	HashMap<int32, PlayerData> _playerData; // Level, Data
	HashMap<std::pair<int32, int32>, SkillData> _skillData;
	HashMap<int32, MonsterData> _monsterData;
	Vector<SpawnData> _spawnData;
};