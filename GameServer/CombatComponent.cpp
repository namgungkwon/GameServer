#include "pch.h"
#include "CombatComponent.h"


///////////////////////////////////////////////////////
//	<	CombatComponent	>
///////////////////////////////////////////////////////
void CombatComponent::Init(int64 damage, int32 defense)
{
	SetDamage(damage);
	SetDefense(defense);
}
