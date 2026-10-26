#include "pch.h"
#include "Monster.h"

#include "StatComponent.h"
#include "SkillComponent.h"
#include "CombatComponent.h"
#include "MovementComponent.h"
#include "AIComponent.h"

void Monster::Init(const SpawnData& spawnData, MonsterData& data)
{
	//Entity
	SetObjectId(IdGenerator::Get());
	SetName(data.name);
	SetLevel(data.level);
	SetLocation(spawnData.location);
	SetYaw(0);
	SetType(Protocol::OBJECT_CATEGORY_MONSTER);
	SetState(EntityState::Idle);
	SetTemplateId(data.id);
	SetExp(data.rewardExp);

	// Component
	AddComponent<StatComponent>(shared_from_this())->Init(data.maxHp, data.maxHp, 0, 0);
	AddComponent<CombatComponent>(shared_from_this())->Init(data.attack, data.defense);
	MovementComponent* mc = AddComponent<MovementComponent>(shared_from_this());
	mc->Init(data.speed);

	AddComponent<AIComponent>(shared_from_this())->Init(
		spawnData.location, spawnData.patrolRadius, data.searchRange, data.maxSearchRange, data.attackRange, data.attackDelayMs, spawnData.respawnMs);
}

void Monster::Respawn(uint64 nowMs)
{
	Entity::Respawn(nowMs);

	AIComponent* ai = GetComponent<AIComponent>();
	SetLocation(GetComponent<AIComponent>()->GetSpawnLocation());
}
