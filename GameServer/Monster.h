#pragma once

#include "Entity.h"

class Monster : public Entity
{
public:
	void Init(const SpawnData& spawnData, MonsterData& data);

	virtual void Respawn(uint64 nowMs) override;
};

