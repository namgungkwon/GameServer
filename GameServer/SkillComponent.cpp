#include "pch.h"
#include "SkillComponent.h"

#include "GameData.h"


///////////////////////////////////////////////////////
//	<	SkillComponent	>
///////////////////////////////////////////////////////
void SkillComponent::Init(const Vector<SkillState>& skills)
{
	for (const SkillState& skill : skills)
		AddSkill(skill);
}

uint64 SkillComponent::UseSkill(uint32 skillId, uint64 now)
{
	SkillData* skillData = GGameData->GetSkillData(skillId, _skills[skillId].skillLevel);
	_skills[skillId].skillCooldownEndTick = now + skillData->cooldownMs;
	return skillData->moveDelayMs;
}

SkillData* SkillComponent::GetSkill(uint32 skillId)
{
	return GGameData->GetSkillData(skillId, _skills[skillId].skillLevel);
}

bool SkillComponent::CooldownReady(uint32 skillId, uint64 now)
{
	if (HasSkill(skillId) == false)
		return false;

	return _skills[skillId].skillCooldownEndTick <= now;
};