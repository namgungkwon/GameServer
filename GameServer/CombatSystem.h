#pragma once

#include "Protocol.pb.h"

struct DamageEvent
{
	uint64 attackerId = 0;
	uint64 targetId = 0;

	uint64 skillId = 0;
	uint64 skillLevel = 0;

	int64 rawDamage = 0;
	int64 defense = 0;
	int64 finalDamage = 0;

	float yaw = 0;
};

struct DamageResult
{
	uint64 attckerId = 0;
	uint64 targetId = 0;

	uint64 skillId = 0;

	int64 damage = 0;
	uint64 targetHp = 0;
	uint64 targetmaxHp = 0;

	bool isDead = false;
};

class CombatSystem
{
public:
	static void Update(ZoneRef zone, float deltaTime);

public:
	static DamageEvent MakeDamageEvent(EntityRef attacker, EntityRef target, uint64 skillId, float yaw);
	static bool ApplyDamage(ZoneRef zone, DamageEvent& event);
	static const DamageResult MakeDamageResult(ZoneRef zone, DamageEvent& event);

public:
	static Vector<EntityRef> SelectTargetInAttackCone(EntityRef attacker, Vector<EntityRef> targets, float AttackRange, float AttackAngle);

private:
	static uint64 CalculateDamage(DamageEvent& event);
};