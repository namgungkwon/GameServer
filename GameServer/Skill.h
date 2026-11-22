#pragma once

#include "CombatSystem.h"

class SkillEffect : public enable_shared_from_this<SkillEffect>
{
public:
	virtual SkillEffectRef Clone() abstract;

public:
	virtual void RunEffect(EntityRef owner, Vector<EntityRef>& targets, int32 skillId) abstract;
};

struct MeleeAttackInfo
{
	int32 attackRate = 0;
	int32 attackRange = 0;
	int32 attackAngle = 90;
	int32 moveDelayMs = 500;
	int32 hitDelayMs = 200;
};

class MeleeAttackEffect : public SkillEffect
{
public:
	MeleeAttackEffect(MeleeAttackInfo attackInfo) : info(attackInfo) {};

public:
	virtual SkillEffectRef Clone() { return MakeShared<MeleeAttackEffect>(*this); }
public:
	virtual void RunEffect(EntityRef owner, Vector<EntityRef>& targets, int32 skillId) override;
	virtual void Execute(GameScene* scene, DamageEvent event, const EntityRef target);

public:
	MeleeAttackInfo info;
};

class SkillComponent;

class SkillDefinition
{
	friend class SkillInstance;

private:
	GETSET(int32, _skillId, SkillId);
	GETSET_REF(string, _name, Name);
	GETSET(uint64, _coolDownMs, CoolDown);

public:
	Vector<Vector<SkillEffectRef>> effectFactory; // [combo, effects]
	Vector<uint64> comboWindowMsList;
};

class SkillInstance
{
public:
	SkillInstance(EntityRef owner, SkillDefinitionRef definition);

public:
	bool AddCombo(uint64 now);
	bool Execute(Vector<EntityRef>& targets);

private:
	std::weak_ptr<Entity> _owner;
	GETSET(SkillDefinitionRef, _definition, Definition);

private:
	int32 _combo = -1;
	uint32 _comboEndTick = 0;
};