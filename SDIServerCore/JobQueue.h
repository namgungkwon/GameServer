#pragma once

#include "Job.h"
#include "LockQueue.h"
#include "JobTimer.h"


///////////////////////////////////////////////////////
//	<	JobQueue	>
///////////////////////////////////////////////////////
class JobQueue : public enable_shared_from_this<JobQueue>
{
	friend class GlobalQueue;
	friend class JobTimer;

public:
	void DoAsync(Job job)
	{
		Push(MakeShared<Job>(std::move(job)));
	}

	void DoAsync(CallbackType&& callback)
	{
		Push(ObjectPool<Job>::MakeShared(std::move(callback)));
	}

	template<typename T, typename Ret, typename... Args>
	void DoAsync(Ret(T::* memfunc)(Args...), Args... args)
	{
		shared_ptr<T> owner = static_pointer_cast<T>(shared_from_this());
		Push(ObjectPool<Job>::MakeShared(owner, memfunc, std::forward<Args>(args)...));
	}

	void DoTimer(uint64 tickAfter, CallbackType&& callback)
	{
		JobRef job = ObjectPool<Job>::MakeShared(std::move(callback));
		GJobTimer->Reserve(tickAfter, shared_from_this(), job);
	}

	template<typename T, typename Ret, typename... Args>
	void DoTimer(uint64 tickAfter, Ret(T::* memfunc)(Args...), Args... args)
	{
		shared_ptr<T> owner = static_pointer_cast<T>(shared_from_this());
		JobRef job = ObjectPool<Job>::MakeShared(owner, memfunc, std::forward<Args>(args)...);
		GJobTimer->Reserve(tickAfter, shared_from_this(), job);
	}

	void				ClearJob() { _jobs.Clear(); }

private:
	void				Push(JobRef job/*, bool pushOnly = false*/);
	void				Execute();

protected:
	LockQueue<JobRef>	_jobs;
	Atomic<int32>		_jobCount = 0;
};

