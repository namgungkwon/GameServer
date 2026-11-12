#include "pch.h"
#include "MovementComponent.h"


///////////////////////////////////////////////////////
//	<	MovementComponent	>
///////////////////////////////////////////////////////
void MovementComponent::Init(float moveSpeed)
{
	pendingJob = MoveJob();
	SetMoveSpeed(moveSpeed);
	_moveDelayEndTick = 0;
};