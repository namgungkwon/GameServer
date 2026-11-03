#pragma once

#include "ConfigLoader.h"

extern class SessionManager* GSessionManager;
extern class DBManager* GDBManager;
extern class RedisManager* GRedisManager;


class GameGlobal
{
public:
	static void Init();
	static void Clear();

	static ServerServiceRef& GetService() { return _service; }

private:
	static GameConfig _config;
	static std::shared_ptr<class ServerService> _service;
	static std::shared_ptr<class DBManager> _dbManager;
	static std::shared_ptr<class RedisManager> _redisManager;
	static std::shared_ptr<class SessionManager> _sessionManager;
};