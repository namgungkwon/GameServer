#pragma once
#include "Entity.h"

struct MoveJob
{
	Vector3 location;
	Vector3 velocity;
	float yaw;
};


///////////////////////////////////////////////////////
//	<	MovementComponent	>
///////////////////////////////////////////////////////
class MovementComponent final : public IObjectComponent
{
public:
	MovementComponent(EntityRef& owner) : IObjectComponent(owner) {};
public:
	void Init(float moveSpeed);

public:
	bool			CanMove(uint64 now)					{ return now > _moveDelayEndTick; }

	void			SetPendingJob(const MoveJob& job)	{ WRITE_LOCK; pendingJob = job; }
	MoveJob&		GetPendingJob()						{ WRITE_LOCK; return pendingJob; }

	void			SetMoveSpeed(float moveSpeed)		{ _moveSpeed = UtilMath::Max(moveSpeed, 0.f); }
	float			GetMoveSpeed()						{ return _moveSpeed; }

	void			SetMoveDelayEndTick(uint64 endTick) { if (endTick > _moveDelayEndTick) _moveDelayEndTick = endTick; }
	uint64			GetMoveDelayEndTick()				{ return _moveDelayEndTick; }

private:
	USE_LOCK;
	MoveJob			pendingJob			= MoveJob();
	float			_moveSpeed			= 0.f;
	uint64			_moveDelayEndTick	= 0;// 스킬을 썼을때나 이상이 있을 때 moveDelay를 설정
};