#pragma once
#include "GameStruct.h"

template<>
struct std::hash<std::pair<uint32, uint32>>
{
	size_t operator()(const std::pair<uint32, uint32>& key) const noexcept
	{
		return std::hash<uint64>{}(
			(static_cast<uint64>(key.first) << 32) ^ static_cast<uint32>(key.second)
			);
	}
};

///////////////////////////////////////////////////////
//	<	GameData	>
///////////////////////////////////////////////////////
class GameData
{
public:
	bool						LoadData();

private:
	bool						LoadPlayerData();
	bool						LoadSkillData();
	bool						LoadMonsterSpawnData();
	bool						LoadMonsterData();
	bool						LoadSpawnPointData();

public:
	PlayerData*					GetPlayerData(int32 level);
	SkillData*					GetSkillData(uint32 skillId, uint32 skillLevel);
	Vector<MonsterSpawnData>&	GetMonsterSpawnData() {	return _monsterSpawnData; }
	MonsterData*				GetMonsterData(uint64 monsterId);

public:
	SpawnPointData*				GetSpawnPoint(const Vector3& currentLocation);

private:
	HashMap<int32, PlayerData>						_playerData; // Level, Data
	HashMap<std::pair<uint32, uint32>, SkillData>	_skillData;
	HashMap<uint64, MonsterData>					_monsterData;
	Vector<MonsterSpawnData>						_monsterSpawnData;
	Vector<SpawnPointData>							_spawnPointData;
};