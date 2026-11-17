#pragma once
#include "Entity.h"


struct SkillState
{
	uint32 skillId = 1;
	uint32 skillLevel = 1;
	uint64 skillCooldownEndTick = 0;
};

///////////////////////////////////////////////////////
//	<	SkillComponent	>
///////////////////////////////////////////////////////
class SkillComponent final : public IObjectComponent
{
public:
	SkillComponent(EntityRef& owner) : IObjectComponent(owner) {};

public:
	void				Init(const Vector<SkillState>& skills);

public:
	void				AddSkill(const SkillState& skill)	{ _skills[skill.skillId] = skill; }
	SkillState&			GetSkillState(uint32 skillId)		{ return _skills[skillId]; }
	bool				HasSkill(uint32 skillId)			{ return _skills.find(skillId) != _skills.end(); }
	SkillInstanceRef	UseSkill(uint32 skillId, uint64 now);
	bool				CooldownReady(uint32 skillId, uint64 now);

private:
	SkillDefinitionRef		GetSkill(uint32 skillId);

private:
	HashMap<uint32, SkillState>		_skills;
	SkillInstanceRef				_castingSkill;
};