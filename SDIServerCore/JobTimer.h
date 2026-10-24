#pragma once


///////////////////////////////////////////////////////
//	<	JobData	>
///////////////////////////////////////////////////////
struct JobData
{
	JobData(weak_ptr<JobQueue> owner, JobRef job) : owner(owner), job(job) {};


	weak_ptr<JobQueue>				owner;
	JobRef							job;
};



///////////////////////////////////////////////////////
//	<	TimerItem	>
///////////////////////////////////////////////////////
struct TimerItem
{
	bool operator<(const TimerItem& other) const
	{
		return executeTick > other.executeTick;
	}

	uint64							executeTick = 0;
	JobData*						jobData = nullptr;
};



///////////////////////////////////////////////////////
//	<	JobTimer	>
///////////////////////////////////////////////////////
class JobTimer
{
public:
	void							Start() { _isStart = true; }
	void							Stop() { _isStart = false; _cv.notify_all(); }

public:
	void							Reserve(uint64 tickAfter, weak_ptr<JobQueue> owner, JobRef job);
	void							Distribute(uint64 now);
	void							Clear();

private:
	USE_LOCK;
	PriorityQueue<TimerItem>		_items;
	bool							_isStart = true;

private:
	CondVar							_cv;
	Mutex							_mutex;
};

