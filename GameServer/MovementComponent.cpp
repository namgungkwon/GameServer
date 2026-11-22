#include "pch.h"
#include "MovementComponent.h"


///////////////////////////////////////////////////////
//	<	MovementComponent	>
///////////////////////////////////////////////////////
void MovementComponent::Init(float moveSpeed)
{
	_pendingJob.push_back(MoveJob());
	SetMoveSpeed(moveSpeed);
	_moveDelayEndTick = 0;
}
void MovementComponent::PushPendingJob(MoveJob job)
{
	WRITE_LOCK;

	if (job.sendClientTick < _pendingJob.back().sendClientTick)
		return;

	_pendingJob.push_back(std::move(job));
}

Vector<MoveJob> MovementComponent::GetPendingJob()
{
	WRITE_LOCK;
	Vector<MoveJob> ret;


	if (_pendingJob.size() == 1)
	{
		ret.push_back(_pendingJob[0]);
	}
	else
	{
		MoveJob lastJob = _pendingJob.back();

		const size_t size = _pendingJob.size();
		ret.reserve(size);
		for (size_t i = 0; i < size; ++i)
		{
			if (i == size - 2)
				_lastMoveProcessedClientTick = _pendingJob[i].sendClientTick;
			ret.push_back(std::move(_pendingJob[i]));
		}
		_pendingJob.clear();
		_pendingJob.push_back(lastJob);
	}

	return ret;
}
