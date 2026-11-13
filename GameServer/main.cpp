#include "pch.h"
#include "ThreadManager.h"
#include "ServerStats.h"

int main()
{
	GameGlobal::Init();

	ServerServiceRef& service = GameGlobal::GetService();
	
	const size_t networkThreadCount = 10;
	for (size_t i = 0; i < networkThreadCount; ++i)
	{
		GThreadManager->Launch([&service]()
			{
				while (true)
				{
					PROFILE_BEGIN(L"service::Process");
					service->Process();
					PROFILE_END(L"service::Process");
				}
			});
	}
	
	const size_t LogicThreadCount = 16;
	for (size_t i = 0; i < LogicThreadCount; ++i)
	{
		GThreadManager->Launch([&service]()
			{
				ThreadManager::DoGlobalQueueWork();
			});
	}


	
	GThreadManager->Launch([]() { if (GServerStats) GServerStats->RunUpdateLoop(); });
	GThreadManager->Launch([]() { if (GProfiler) GProfiler->UpdateProfiler(); });

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