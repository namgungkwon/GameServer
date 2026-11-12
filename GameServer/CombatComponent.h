#pragma once

#include "Entity.h"


///////////////////////////////////////////////////////
//	<	CombatComponent	>
///////////////////////////////////////////////////////
class CombatComponent final : public IObjectComponent
{
public:
	CombatComponent(EntityRef& owner) : IObjectComponent(owner) {};
public:
	void			Init(int64 damage, int32 defense);

	void			SetDamage(int64 damage)		{ _damage = UtilMath::Max(damage, (int64)0); }
	int64			GetDamage()					{ return _damage; }

	void			SetDefense(int32 defense)	{ _defense = UtilMath::Max(defense, 0);}
	int32			GetDefense()				{ return _defense; }


private:
	int64			_damage		= 0;
	int32			_defense	= 0;
};