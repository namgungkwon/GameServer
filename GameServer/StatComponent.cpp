#include "pch.h"
#include "StatComponent.h"

///////////////////////////////////////////////////////
//	<	StatComponent	>
///////////////////////////////////////////////////////

void StatComponent::Init(int64 currentHp, int64 maxHp, int64 currentMp, int64 maxMp)
{
	SetMaxHP	(maxHp);
	SetCurrentHP(currentHp);
	SetMaxMP	(maxMp);
	SetCurrentMP(currentMp);
};
