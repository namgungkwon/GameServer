#include "pch.h"
#include "GameGlobal.h"


#include "Packet.h"
#include "ClientPacketHandler.h"
#include "DBManager.h"
#include "RedisManager.h"
#include "Gamesession.h"
#include "SessionManager.h"

GameConfig GameGlobal::_config = {};
std::shared_ptr<ServerService> GameGlobal::_service = nullptr;
std::shared_ptr<DBManager> GameGlobal::_dbManager = nullptr;
std::shared_ptr<RedisManager> GameGlobal::_redisManager = nullptr;
std::shared_ptr<SessionManager> GameGlobal::_sessionManager = nullptr;

SessionManager* GSessionManager = nullptr;
DBManager* GDBManager = nullptr;
RedisManager* GRedisManager = nullptr;

void GameGlobal::Init()
{
	// CoreGlobal
	CoreGlobal::Init();

	// ServerConfig
	if (ConfigLoader::Load("../GameServer/server.json", _config))
		LOG_INFO(L"ConfigFile load complete");
	else
		LOG_ERROR(L"Failed to load ConfigFile");

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
	if (GRedisManager->Init(_config.redis))
		LOG_INFO(L"RedisManager init complete");
	else
		LOG_ERROR(L"Failed to init RedisManager");

	// 8. Session Manager
	_sessionManager = MakeShared<SessionManager>();
	GSessionManager = _sessionManager.get();

	// PacketHandler
	Init_PacketHandler();
	ClientPacketHandler::Init();

	// Service
	_service = MakeShared<ServerService>(
		NetAddress(_config.loginServer.ip, _config.loginServer.port),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>,
		_config.loginServer.maxSessions,
		_config.loginServer.acceptPool
	);
	ASSERT_CRASH(_service->Start());
}


void GameGlobal::Clear()
{

}
