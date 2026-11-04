#include "pch.h"

#include "ThreadManager.h"


enum
{
	WORKER_TICK = 64,
};

void DBTest();

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


	DBTest();

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

#include <chrono>
#include "DBManager.h"
#include "RedisManager.h"
#include "Job.h"
#include "Protocol.pb.h"

void DBTest()
{
/*	GDBManager->DoAsync(Job([]() {

		uint64 accountId;
		Vector<Protocol::ObjectSummaryData> summaries;
		GDBManager->LoginGame("admin", "1", accountId);


		Protocol::Result result = GDBManager->LoginGame("admin", "1", accountId);

		if (result != Protocol::SUCCESS)
		{
			Protocol::LC_LOGIN res;
			res.set_result(result);
			return;
		}

		GRedisManager->DoAsync([accountId]()
			{
				std::string token = GRedisManager->SetToken(accountId);

				Protocol::LC_LOGIN res;
				res.set_result(Protocol::SUCCESS);
				res.set_token(token);
			});

		}));*/
}
