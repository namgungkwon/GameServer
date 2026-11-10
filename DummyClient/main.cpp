#include "pch.h"

#include "LoginServerPacketHandler.h"
#include "GameServerPacketHandler.h"
#include "DummyManager.h"

int main()
{
	CoreGlobal::Init();

	{
		Init_PacketHandler();
		GameServerPacketHandler::Init();
		LoginServerPacketHandler::Init();

		DummyManagerRef dummyManager = MakeShared<DummyManager>();

		dummyManager->Start();


		while (true)
		{
			LOG_INFO(L"inert Command : (add - add dummy), (q - quit): ");
			string str;
			cin >> str;
			if (str == "q")
			{
				break;
			}

			if (str == "add")
			{
				LOG_INFO(L"dummyCount : ");
				cin >> str;

				int32 count = std::atoi(str.c_str());

				dummyManager->DoAsync(&DummyManager::AddDummyClient, count);
			}
		}

		dummyManager->DoAsync(&DummyManager::Clear);
	}

	CoreGlobal::Clear();

	return 0;
}