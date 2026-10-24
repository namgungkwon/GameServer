#pragma once
#include "Entity.h"

#include "GameData.h"

struct SkillState
{
	uint64 skillId = 1;
	uint64 skillLevel = 1;
	uint64 skillCooldownEndTick = 0;
};

class SkillComponent final : public IObjectComponent
{
public:
	SkillComponent(EntityRef& owner) : IObjectComponent(owner) {};
public:

	void Init(Vector<SkillState>& skills)
	{
		for (SkillState& skill : skills)
			AddSkill(skill);
	}

	void AddSkill(SkillState& skill)
	{
		_skills[skill.skillId] = skill;
	}

	bool HasSkill(uint64 skillId) { return _skills.find(skillId) != _skills.end(); }

	uint64 UseSkill(uint64 skillId, uint64 now)
	{
		SkillData* skillData = GGameData->GetSkillData(skillId, _skills[skillId].skillLevel);
		_skills[skillId].skillCooldownEndTick = now + skillData->cooldownMs;
		return skillData->moveDelayMs;
	}

	bool CooldownReady(uint64 skillId, uint64 now)
	{
		if (HasSkill(skillId) == false)
			return false;

		return _skills[skillId].skillCooldownEndTick <= now;
	};

	SkillState& GetSkillState(uint64 skillId) { return _skills[skillId]; }

private:
	HashMap<uint64, SkillState> _skills;
};