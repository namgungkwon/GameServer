#include "pch.h"
#include "ThreadManager.h"
#include "ServerStats.h"

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
	
	const size_t LogicThreadCount = 1;
	for (size_t i = 0; i < LogicThreadCount; ++i)
	{
		GThreadManager->Launch([&service]()
			{
				ThreadManager::DoGlobalQueueWork();
			});
	}


	
	GThreadManager->Launch([]() { if (GServerStats) GServerStats->RunUpdateLoop(); });

	while (true)
	{
		char ch;
		cin >> ch;
		if (ch == 'q')
		{
			break;
		}
	}

	GameGlobal::Clear();

	return 0;
}