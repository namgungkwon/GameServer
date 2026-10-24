#pragma once

#include "Entity.h"

class StatComponent final : public IObjectComponent
{
public:
	StatComponent(EntityRef& owner) : IObjectComponent(owner) {};
public:
	void Init(int32 currentHp, int32 maxHp, int32 currentMp, int32 maxMp)
	{
		SetMaxHP(maxHp);
		SetCurrentHP(currentHp);
		SetMaxMP(maxMp);
		SetCurrentMP(currentMp);
	};

public:
	int32 GetCurrentHP() { return _currentHP; }
	void SetCurrentHP(int32 hp) { _currentHP = UtilMath::Clamp(hp, 0, _maxHP); }

	int32 GetMaxHP() { return _maxHP; }
	void SetMaxHP(int32 hp) { _maxHP = UtilMath::Max(hp, 1); }

	int32 GetCurrentMP() { return _currentMP; }
	void SetCurrentMP(int32 mp) { _currentMP = UtilMath::Clamp(mp, 0, _maxMP); }

	int32 GetMaxMP() { return _maxMP; }
	void SetMaxMP(int32 mp) { _maxMP = UtilMath::Max(mp, 1); }

private:
	int32 _currentHP;
	int32 _maxHP;

	int32 _currentMP;
	int32 _maxMP;
};