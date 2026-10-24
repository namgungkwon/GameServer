#pragma once

#include "Entity.h"

class CombatComponent final : public IObjectComponent
{
public:
	CombatComponent(EntityRef& owner) : IObjectComponent(owner) {};
public:
	void Init(int32 damage, int32 defense)
	{
		SetDamage(damage);
		SetDefense(defense);
	}

	void SetDamage(int32 damage) { _damage = UtilMath::Max(damage, 0); }
	int32 GetDamage() { return _damage; }

	void SetDefense(int32 defense) { _defense = UtilMath::Max(defense, 0);}
	int32 GetDefense() { return _defense; }


private:
	int32 _damage;
	int32 _defense;
};