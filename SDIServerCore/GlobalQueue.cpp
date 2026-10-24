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
	JobQueueRef jobQueue = Pop();
	if (jobQueue == nullptr)
	{
		UniqueLock lock(_mutex);
		_cv.wait(lock, [this] {return CheckEmpty() == false || _isStart == false; });
		return;
	}

	jobQueue->Execute();
}