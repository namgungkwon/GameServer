#include "pch.h"
#include "GlobalQueue.h"

#include "JobQueue.h"


///////////////////////////////////////////////////////
//	<	GlobalQueue	>
///////////////////////////////////////////////////////
GlobalQueue::GlobalQueue()
{

}

GlobalQueue::~GlobalQueue()
{

}

void GlobalQueue::Push(JobQueueRef jobQueue)
{
	_jobQueues.Push(jobQueue);
	_cv.notify_one();
}

JobQueueRef GlobalQueue::Pop()
{
	return _jobQueues.Pop();
}

bool GlobalQueue::CheckEmpty()
{
	return _jobQueues.IsEmpty();
}

void GlobalQueue::Execute()
{
	while (true)
	{
		JobQueueRef jobQueue = Pop();

		if (jobQueue)
		{
			jobQueue->Execute();
			continue;
		}

		UniqueLock lock(_mutex);

		_cv.wait(lock, [this] {return _isStart == false || CheckEmpty() == false; });

		if (_isStart == false)
			break;
	}
}