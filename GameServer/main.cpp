#include "pch.h"
#include "ThreadManager.h"

enum
{
	WORKER_TICK = 64,
};

int main()
{
	GameGlobal::Init();

	ServerServiceRef& service = GameGlobal::GetService();

	const size_t networkThreadCount = 5;
	for (size_t i = 0; i < networkThreadCount; ++i)
	{
		GThreadManager->Launch([&service]()
			{
				while (true)
					service->Process();
			});
	}

	const size_t LogicThreadCount = 5;
	for (size_t i = 0; i < LogicThreadCount; ++i)
	{
		GThreadManager->Launch([&service]()
			{
				LEndTickCount = ::GetTickCount64() + WORKER_TICK;
				while (true)
					ThreadManager::DoGlobalQueueWork();
			});
	}


	while (true)
	{
		char ch;
		cin >> ch;
		if (ch == 'q')
		{
			break;
		}
	}


	return 0;
}