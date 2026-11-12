#pragma once

#include "Protocol.pb.h"

struct DamageEvent
{
	uint64		attackerId		= 0;
	uint64		targetId		= 0;

	uint32		skillId			= 0;
	uint32		skillLevel		= 0;

	int64		rawDamage		= 0;
	int64		defense			= 0;
	int64		finalDamage		= 0;

	float		yaw				= 0;
};

struct DamageResult
{
	uint64		attckerId		= 0;
	uint64		targetId		= 0;

	uint64		skillId			= 0;

	int64		damage			= 0;
	uint64		targetHp		= 0;
	uint64		targetmaxHp		= 0;

	bool		isDead			= false;
};


///////////////////////////////////////////////////////
//	<	CombatSystem	>
///////////////////////////////////////////////////////
class CombatSystem
{
public:
	static void					Update(ZoneRef zone, float deltaTime);

public:
	static DamageEvent			MakeDamageEvent(const EntityRef& attacker, const EntityRef& target, uint32 skillId, float yaw);
	static bool					ApplyDamage(const ZoneRef& zone, DamageEvent& event);
	static const DamageResult	MakeDamageResult(const ZoneRef& zone, DamageEvent& event);

public:
	static Vector<EntityRef>	SelectTargetInAttackCone(const EntityRef& attacker, Vector<EntityRef>& targets, float AttackRange, float AttackAngle);

private:
	static uint64				CalculateDamage(DamageEvent& event);
};