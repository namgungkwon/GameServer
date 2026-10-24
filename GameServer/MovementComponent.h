#pragma once
#include "Entity.h"

class MovementComponent final : public IObjectComponent
{
public:
	MovementComponent(EntityRef& owner) : IObjectComponent(owner) {};
public:
	void Init(float moveSpeed)
	{
		_velocity = Vector3();
		SetMoveSpeed(moveSpeed);
		_moveDelayEndTick = 0;
	};

public:
	void SetVelocity(Vector3 velocity) { _velocity = velocity; }
	Vector3 GetVelocity() { return _velocity; }

	void SetMoveSpeed(float moveSpeed) { _moveSpeed = UtilMath::Max(moveSpeed, 0.f); }
	float GetMoveSpeed() { return _moveSpeed; }

	void SetMoveDelayEndTick(uint64 endTick) { _moveDelayEndTick = endTick; }
	uint64 GetMoveDelayEndTick() { return _moveDelayEndTick; }

private:
	Vector3 _velocity;
	float _moveSpeed = 0.f;
	uint64 _moveDelayEndTick = 0;// 스킬을 썼을때나 이상이 있을 때 moveDelay를 설정
};