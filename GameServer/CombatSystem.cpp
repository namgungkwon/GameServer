#include "pch.h"
#include "CombatSystem.h"
#include "Entity.h"
#include "GameData.h"
#include "Zone.h"

#include "SkillComponent.h"
#include "CombatComponent.h"
#include "StatComponent.h"
#include "GameData.h"

DamageEvent CombatSystem::MakeDamageEvent(EntityRef attacker, EntityRef target, uint64 skillId, float yaw)
{
	DamageEvent event;

	event.attackerId = attacker->GetObjectId();
	event.targetId = target->GetObjectId();

	SkillComponent* sc = attacker->GetComponent<SkillComponent>();
	event.skillId = skillId;
	event.skillLevel = sc->GetSkillState(skillId).skillLevel;

	CombatComponent* attackercc = attacker->GetComponent<CombatComponent>();
	CombatComponent* targetcc = target->GetComponent<CombatComponent>();

	event.rawDamage = attackercc->GetDamage();
	event.defense = targetcc->GetDefense();
	event.finalDamage = CalculateDamage(event);

	event.yaw = yaw;
	return event;
}

bool CombatSystem::ApplyDamage(ZoneRef zone, DamageEvent& event)
{
	EntityRef target = zone->FindEntity(event.targetId);

	if (target == nullptr)
		return false;

	StatComponent* tStat = target->GetComponent<StatComponent>();

	int32 currentHp = tStat->GetCurrentHP();
	const uint32 appliedDamage = event.finalDamage < currentHp ? event.finalDamage : currentHp;

	tStat->SetCurrentHP(currentHp - appliedDamage);

	event.finalDamage = appliedDamage;
	return true;
}

const DamageResult CombatSystem::MakeDamageResult(ZoneRef zone, DamageEvent& event)
{
	DamageResult result;

	result.attckerId = event.attackerId;
	result.targetId = event.targetId;

	result.skillId = event.skillId;

	result.damage = event.finalDamage;

	EntityRef target = zone->FindEntity(event.targetId);
	if (target)
	{
		StatComponent* tStat = target->GetComponent<StatComponent>();
		if (tStat)
		{
			result.targetHp = tStat->GetCurrentHP();
			result.targetmaxHp = tStat->GetMaxHP();

			if (tStat->GetCurrentHP() == 0)
			{
				target->SetState(EntityState::Dead);
			}
		}
	}

	return result;
}

Vector<EntityRef> CombatSystem::SelectTargetInAttackCone(EntityRef attacker, Vector<EntityRef> targets, float AttackRange, float AttackAngle)
{
	if (attacker == nullptr)
		return Vector<EntityRef>();

	Vector<EntityRef> ret;

	Vector3 attackerLocation = attacker->GetLocation();
	Vector3 forwardVector = UtilMath::GetForwardFromYaw(attacker->GetYaw());

	const float halfAngleRadians = AttackAngle * PI / 180.f;

	const float minDot = std::cos(halfAngleRadians);
	const float attackRangeSq = AttackRange * AttackRange;

	for (EntityRef& target : targets)
	{
		if (target == nullptr)
			continue;

		Vector3 targetLocation = target->GetLocation();
		
		float distanceSq = targetLocation.DistanceSq(attackerLocation);

		// 사거리
		if (distanceSq > attackRangeSq)
			continue;

		const float distance =  ::sqrt(distanceSq);

		const Vector3 toTarget = targetLocation / distance;

		// 각도
		const float AngleDot = forwardVector.Dot(toTarget);

		if (AngleDot >= minDot)
		{
			ret.push_back(target);
		}
	}
	return ret;
}

uint64 CombatSystem::CalculateDamage(DamageEvent& event)
{
	SkillData* skill = GGameData->GetSkillData(event.skillId, event.skillLevel);

	uint64 finalDamage = (event.rawDamage * skill->attackRate) * 0.01f;

	finalDamage = event.rawDamage - event.defense;

	if (finalDamage < 0)
		finalDamage = 0;

	return finalDamage;
}