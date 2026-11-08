#include "pch.h"
#include "JobTimer.h"

#include "JobQueue.h"


///////////////////////////////////////////////////////
//	<	JobTimer	>
///////////////////////////////////////////////////////
void JobTimer::Reserve(uint64 tickAfter, weak_ptr<JobQueue> owner, JobRef job)
{
	const uint64 executeTick = ::GetTickCount64() + tickAfter;
	JobData* jobData = ObjectPool<JobData>::Pop(owner, job);

	{
		UniqueLock lock(_mutex);
		_items.push(TimerItem{ executeTick, jobData });
	}

	_cv.notify_one();
}

void JobTimer::Distribute()
{
	while (true)
	{
		uint64 now = GetTickCount64();
		Vector<TimerItem> items;
		uint64 nextWaitTick = INT64_MAX;
		{
			{
				UniqueLock lock(_mutex);

				while (_items.empty() == false)
				{
					const TimerItem& timerItem = _items.top();

					if (now < timerItem.executeTick)
					{
						nextWaitTick = timerItem.executeTick;
						break;
					}

					items.push_back(timerItem);
					_items.pop();
				}
			}

			for (TimerItem& item : items)
			{
				if (JobQueueRef owner = item.jobData->owner.lock())
					owner->Push(item.jobData->job);

				ObjectPool<JobData>::Push(item.jobData);
			}

			{
				UniqueLock lock(_mutex);
				if (_items.empty())
				{
					// 스레드 슬립
					_cv.wait(lock, [this] { return _items.empty() == false || _isStart == false; });
				}
				else
				{
					const uint64 nowTick = ::GetTickCount64();
					const uint64 nextWaitTick = _items.top().executeTick;
					const uint64 delayTime = nextWaitTick > nowTick ? nextWaitTick - nowTick : 0;

					_cv.wait_for(lock, std::chrono::milliseconds(delayTime));
				}
			}

			if (_isStart == false)
				break;
		}
	}
	
}


void JobTimer::Clear()
{
	WRITE_LOCK;
	
	while (_items.empty() == false)
	{
		const TimerItem& timerItem = _items.top();
		ObjectPool<JobData>::Push(timerItem.jobData);
		_items.pop();
	}
}
