#pragma once

#include "Entity.h"


///////////////////////////////////////////////////////
//	<	StatComponent	>
///////////////////////////////////////////////////////
class StatComponent final : public IObjectComponent
{
public:
	StatComponent(EntityRef& owner) : IObjectComponent(owner) {};
public:
	void Init(int64 currentHp, int64 maxHp, int64 currentMp, int64 maxMp);

public:
	int64		GetCurrentHP()			{ return _currentHP; }
	void		SetCurrentHP(int64 hp)	{ _currentHP = UtilMath::Clamp(hp, (int64)0, _maxHP); }

	int64		GetMaxHP()				{ return _maxHP; }
	void		SetMaxHP(int64 hp)		{ _maxHP = UtilMath::Max(hp, (int64)1); }

	int64		GetCurrentMP()			{ return _currentMP; }
	void		SetCurrentMP(int64 mp)	{ _currentMP = UtilMath::Clamp(mp, (int64)0, _maxMP); }

	int64		GetMaxMP()				{ return _maxMP; }
	void		SetMaxMP(int64 mp)		{ _maxMP = UtilMath::Max(mp, (int64)1); }

private:
	int64		_currentHP	= 0;
	int64		_maxHP		= 0;

	int64		_currentMP	= 0;
	int64		_maxMP		= 0;
};