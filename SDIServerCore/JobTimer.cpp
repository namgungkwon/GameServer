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
		const uint64 now = GetTickCount64();
		Vector<TimerItem> items;
		uint64 nextWaitTick = INT64_MAX;
		{
			{
				UniqueLock lock(_mutex);
				// TimerQueue에서 현재 시간보다 이전인 아이템을 빼온다(Priority Queue)
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

			// item의 OwnerJobQueue를 찾아서 밀어넣는다.
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
					// 아이템이 없을 경우 스레드 슬립
					_cv.wait(lock, [this] { return _isStart == false || _items.empty() == false; });
				}
				else
				{
					// 아이템이 있을 경우 새로 들어오거나, 가장 빠른 timer실행 시간까지 슬립
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
