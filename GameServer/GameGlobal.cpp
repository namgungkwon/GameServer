#include "pch.h"
#include "GameGlobal.h"

#include "GameSession.h"
#include "DBManager.h"
#include "RedisManager.h"
#include "SessionManager.h"
#include "GameData.h"
#include "GridMap.h"
#include "World.h"
#include "Packet.h"
#include "ClientPacketHandler.h"

GameConfig GameGlobal::_config = {};
std::shared_ptr<ServerService> GameGlobal::_service = nullptr;
std::shared_ptr<DBManager> GameGlobal::_dbManager = nullptr;
std::shared_ptr<RedisManager> GameGlobal::_redisManager = nullptr;
std::shared_ptr<SessionManager> GameGlobal::_sessionManager = nullptr;
std::shared_ptr<GameData> GameGlobal::_gameData = nullptr;
std::shared_ptr<GridMap> GameGlobal::_gridMap = nullptr;
std::shared_ptr<World> GameGlobal::_world = nullptr;

DBManager* GDBManager = nullptr;
RedisManager* GRedisManager = nullptr;
SessionManager* GSessionManager = nullptr;
GameData* GGameData = nullptr;
GridMap* GGridMap = nullptr;
World* GWorld = nullptr;


void GameGlobal::Init()
{
	// 1. Global
	CoreGlobal::Init();

	// 2. ServerConfig
	if (ConfigLoader::Load("server.json", _config))
		LOG_INFO(L"ConfigFile load complete");
	else
		LOG_ERROR(L"Failed to load ConfigFile");

	// 3. GameData
	_gameData = MakeShared<GameData>();
	GGameData = _gameData.get();
	if (GGameData->LoadData())
		LOG_INFO(L"GameData load complete");
	else
		LOG_ERROR(L"Failed to load GameData");

	// 4. World
	_world = MakeShared<World>();
	GWorld = _world.get();
	GWorld->Init(_config.world);
	GWorld->Start();

	// 5. GridMap
	_gridMap = MakeShared<GridMap>();
	GGridMap = _gridMap.get();
	if (GGridMap->Init("../MapFile/Map.txt"))
		LOG_INFO(L"GridMap load complete");
	else
		LOG_WARN(L"Failed to load GridMap");

	// 6. Database Manager
	_dbManager = MakeShared<DBManager>();
	GDBManager = _dbManager.get();
	if (GDBManager->Init(_config.db))
		LOG_INFO(L"DBManager init complete");
	else
		LOG_ERROR(L"Failed to init DBManager");

	// 7. Redis Manager
	_redisManager = MakeShared<RedisManager>();
	GRedisManager = _redisManager.get();
	if (GRedisManager->Init(_config.redis, _config.server))
		LOG_INFO(L"RedisManager init complete");
	else
		LOG_ERROR(L"Failed to init RedisManager");

	// 8. Session Manager
	_sessionManager = MakeShared<SessionManager>();
	GSessionManager = _sessionManager.get();

	// 9. PacketHandler
	Init_PacketHandler();
	ClientPacketHandler::Init();

	// 9. Service
	_service = MakeShared<ServerService>(
		NetAddress(_config.server.ip, _config.server.port),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>,
		_config.server.acceptPool
	);
	ASSERT_CRASH(_service->Start());


	LOG_INFO(L"=== [Server Ready to Accept Connections] ===");
}


void GameGlobal::Clear()
{
	LOG_INFO(L"Server shutting down...");

	GSessionManager = nullptr;
	_sessionManager = nullptr;

	GRedisManager = nullptr;
	_redisManager = nullptr;

	GDBManager = nullptr;
	_dbManager = nullptr;

	GGridMap = nullptr;
	_gridMap = nullptr;

	GGameData = nullptr;
	_gameData = nullptr;

	CoreGlobal::Clear();
}
