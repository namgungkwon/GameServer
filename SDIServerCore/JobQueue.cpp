#include "pch.h"
#include "JobQueue.h"

#include "GlobalQueue.h"


///////////////////////////////////////////////////////
//	<	JobQueue	>
///////////////////////////////////////////////////////
void JobQueue::Push(JobRef job)
{
	const int32 prevCount = _jobCount.fetch_add(1);
	_jobs.Push(job);

	// 이전에 큐가 비어있었다면 실행할 수 있게 글로벌 큐에 밀어넣음
	if (prevCount == 0)
	{
		GGlobalQueue->Push(shared_from_this());
	}
}

void JobQueue::Execute()
{
	LEndTickCount = GetTickCount64() + WORKER_TICK;
	while (true)
	{
		Vector<JobRef> jobs;
		_jobs.PopAll(OUT jobs);

		// 예약된 job을 전부 꺼내서 실행
		const int32 jobCount = static_cast<int32>(jobs.size());
		for (int32 i = 0; i < jobCount; ++i)
		{
			jobs[i]->Execute();
		}

		if (GServerStats)
		{
			GServerStats->job.jobsExecuted.fetch_add(jobCount);
		}

		// 실행한 job만큼 count를 감소시키고, 모두 처리 되었을 경우 반복문을 나감
		if (_jobCount.fetch_sub(jobCount) == jobCount)
		{
			break;
		}

		// 남아있을 경우 현재 시칸을 체크해서 시간이 많이 지났을 경우 다시 GlobalQueue에 넣어서 다른 스레드가 처리할 수 있게 함
		const uint64 now = ::GetTickCount64();
		if (now >= LEndTickCount)
		{
			GGlobalQueue->Push(shared_from_this());
			break;
		}
	}

}
