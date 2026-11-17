#include "pch.h"
#include "SkillComponent.h"
#include "Skill.h"

#include "GameData.h"


///////////////////////////////////////////////////////
//	<	SkillComponent	>
///////////////////////////////////////////////////////
void SkillComponent::Init(const Vector<SkillState>& skills)
{
	for (const SkillState& skill : skills)
		AddSkill(skill);
}

SkillInstanceRef SkillComponent::UseSkill(uint32 skillId, uint64 now)
{
	do
	{
		if (_castingSkill == nullptr)
			break;

		if (_castingSkill->GetDefinition()->GetSkillId() != skillId)
		{
			_castingSkill = nullptr;
			break;
		}

		if (_castingSkill->AddCombo(now) == false)
		{
			_castingSkill = nullptr;
			break;
		}

		return _castingSkill;
	} while (false);
	
	SkillDefinitionRef skillDef = GetSkill(skillId);
	_skills[skillId].skillCooldownEndTick = now + skillDef->GetCoolDown();
	_castingSkill = MakeShared<SkillInstance>(_owner.lock(), skillDef);
	_castingSkill->AddCombo(now);

	return _castingSkill;
}

SkillDefinitionRef SkillComponent::GetSkill(uint32 skillId)
{
	return GGameData->GetSkillData(skillId, _skills[skillId].skillLevel);
}

bool SkillComponent::CooldownReady(uint32 skillId, uint64 now)
{
	if (HasSkill(skillId) == false)
		return false;

	return _skills[skillId].skillCooldownEndTick <= now;
};