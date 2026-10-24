#include "pch.h"
#include "ThreadManager.h"

#include "GlobalQueue.h"
#include "JobTimer.h"


///////////////////////////////////////////////////////
//	<	ThreadManager	>
///////////////////////////////////////////////////////
ThreadManager::ThreadManager()
{
	InitTLS();

	Launch([]() {	while (true) DistributeReserveJobs(); });
}

ThreadManager::~ThreadManager()
{
	Join();
}

void ThreadManager::Launch(function<void(void)> callback)
{
	LockGuard guard(_lock);

	_threads.push_back(
		thread(
			[=]() {
				InitTLS();
				callback();
				DestroyTLS();
			}
		));
}

void ThreadManager::Join()
{
	for (thread& t : _threads)
	{
		if (t.joinable())
			t.join();
	}
	_threads.clear();
}

void ThreadManager::InitTLS()
{
	static Atomic<uint32> SThreadId = 0;
	LThreadId = SThreadId.fetch_add(1);
}

void ThreadManager::DestroyTLS()
{

}

void ThreadManager::DoGlobalQueueWork()
{
	GGlobalQueue->Execute();
}

void ThreadManager::DistributeReserveJobs()
{
	const uint64 now = ::GetTickCount64();

	GJobTimer->Distribute(now);
}
