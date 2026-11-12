#include "pch.h"
#include "CombatSystem.h"
#include "Entity.h"
#include "GameData.h"
#include "Zone.h"

#include "SkillComponent.h"
#include "CombatComponent.h"
#include "StatComponent.h"
#include "GameData.h"


///////////////////////////////////////////////////////
//	<	CombatSystem	>
///////////////////////////////////////////////////////
DamageEvent CombatSystem::MakeDamageEvent(const EntityRef& attacker, const EntityRef& target, uint32 skillId, float yaw)
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

bool CombatSystem::ApplyDamage(const ZoneRef& zone, DamageEvent& event)
{
	EntityRef target = zone->FindEntity(event.targetId);

	if (target == nullptr)
		return false;

	StatComponent* tStat = target->GetComponent<StatComponent>();

	const int64 currentHp = tStat->GetCurrentHP();
	const int64 appliedDamage = event.finalDamage < currentHp ? event.finalDamage : currentHp;

	tStat->SetCurrentHP(currentHp - appliedDamage);

	event.finalDamage = appliedDamage;
	return true;
}

const DamageResult CombatSystem::MakeDamageResult(const ZoneRef& zone, DamageEvent& event)
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
				result.isDead = true;
				target->SetState(EntityState::Dead);
			}
		}
	}

	return result;
}

Vector<EntityRef> CombatSystem::SelectTargetInAttackCone(const EntityRef& attacker, Vector<EntityRef>& targets, float AttackRange, float AttackAngle)
{
	if (attacker == nullptr)
		return Vector<EntityRef>();

	Vector<EntityRef> ret;

	const Vector3 attackerLocation = attacker->GetLocation();
	const Vector3 forwardVector = UtilMath::GetForwardVectorFromYaw(attacker->GetYaw());

	const float halfAngleRadians = AttackAngle * 0.5f * static_cast<float>(PI / 180.f);

	const float minDot = std::cos(halfAngleRadians);
	const float attackRangeSq = AttackRange * AttackRange;

	for (EntityRef& target : targets)
	{
		if (target == nullptr)
			continue;

		const Vector3 targetLocation = target->GetLocation();
		const float distanceSq = targetLocation.DistanceSq(attackerLocation);

		// 사거리
		if (distanceSq > attackRangeSq)
			continue;

		const float distance =  ::sqrt(distanceSq);
		if (distance <= 0.01f)
		{
			ret.push_back(std::move(target));
			continue;
		}

		const Vector3 toTarget = (targetLocation - attackerLocation) / distance;

		// 각도
		const float AngleDot = forwardVector.Dot(toTarget);
		const float Angle = std::acos(AngleDot) * static_cast < float>(180 / PI);
		if (AngleDot >= minDot)
		{
			ret.push_back(std::move(target));
		}
	}
	return ret;
}

uint64 CombatSystem::CalculateDamage(DamageEvent& event)
{
	SkillData* skill = GGameData->GetSkillData(event.skillId, event.skillLevel);

	uint64 finalDamage = static_cast<uint64>((event.rawDamage * skill->attackRate) * 0.01f);

	finalDamage = event.rawDamage - event.defense;

	if (finalDamage < 0)
		finalDamage = 0;

	return finalDamage;
}