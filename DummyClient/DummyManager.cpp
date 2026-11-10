#include "pch.h"
#include "DummyManager.h"
#include "DummyClient.h"

#include "LoginServerSession.h"
#include "GameServerSession.h"

#include "ThreadManager.h"

void DummyManager::Start()
{
	_loginServerService = MakeShared<ClientService>(
		NetAddress(L"127.0.0.1", 8001),
		MakeShared<IocpCore>(),
		MakeShared<LoginServerSession>,
		3000);

	_gameServerService = MakeShared<ClientService>(
		NetAddress(L"127.0.0.1", 8000),
		MakeShared<IocpCore>(),
		MakeShared<GameServerSession>,
		3000);

	ASSERT_CRASH(_loginServerService->Start());
	ASSERT_CRASH(_gameServerService->Start());

	_isStart.store(true);
	std::weak_ptr<ClientService> loginService = _loginServerService;
	std::weak_ptr<ClientService> gameService = _gameServerService;

	for (size_t i = 0; i < 5; ++i)
	{
		GThreadManager->Launch([this, loginService]() {
			while (_isStart.load() == true)
			{
				if (ClientServiceRef login = loginService.lock())
					login->Process();
			}
			});
	}

	for (size_t i = 0; i < 5; ++i)
	{
		GThreadManager->Launch([this, gameService]() {
			while (_isStart.load() == true)
			{
				if (ClientServiceRef game = gameService.lock())
					game->Process();
			}
			});
	}

	for (size_t i = 0; i < 5; ++i)
	{
		GThreadManager->Launch([]() { ThreadManager::DoGlobalQueueWork(); });
	}
	
	UpdateTick();
}

void DummyManager::Clear()
{
	_isStart.store(false);

	for (DummyClientRef& dummy : dummyList)
	{
		dummy->Clear();
	}

	dummyList.clear();

	_loginServerService = nullptr;
	_gameServerService = nullptr;
}

void DummyManager::UpdateTick()
{
	if (_isStart.load() == true)
		DoTimer(DummyTick, &DummyManager::UpdateTick);

	for (DummyClientRef& dummy : dummyList)
	{
		dummy->Process();
	}
}

void DummyManager::AddDummyClient(int32 count)
{
	if (_isStart.load() == false)
		return;


	for (size_t i = 0; i < count; ++i)
	{
		DummyClientRef dummy = MakeShared<DummyClient>();
		string id = "test" + std::to_string(dummyList.size());
		string password = "1";
		dummy->Init(_gameServerService, _loginServerService, id, password);
		dummyList.push_back(dummy);

		DoTimer(500, [dummy]() {dummy->Login(); });
	}
}