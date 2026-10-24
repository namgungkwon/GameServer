#pragma once


///////////////////////////////////////////////////////
//	<	GlobalQueue	>
///////////////////////////////////////////////////////
class GlobalQueue
{
public:
	GlobalQueue();
	~GlobalQueue();

public:
	void							Start() { _isStart = true; }
	void							Stop() { _isStart = false; _cv.notify_all(); }

public:
	void			Push(JobQueueRef jobQueue);
	JobQueueRef		Pop();
	bool			CheckEmpty();

public:
	void			Execute();

private:
	LockQueue<JobQueueRef> _jobQueues;

private:
private:
	CondVar							_cv;
	Mutex							_mutex;
	bool							_isStart;
};

