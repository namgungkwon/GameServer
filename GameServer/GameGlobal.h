#pragma once

#include "ConfigLoader.h"

extern class DBManager* GDBManager;
extern class RedisManager* GRedisManager;
extern class SessionManager* GSessionManager;
extern class GameData* GGameData;
extern class GridMap* GGridMap;
extern class World* GWorld;

class GameGlobal
{
public:
	static void Init();
	void Clear();

	static ServerServiceRef& GetService() { return _service; }

private:
	static GameConfig _config;
	static std::shared_ptr<class ServerService> _service;
	static std::shared_ptr<class DBManager> _dbManager;
	static std::shared_ptr<class RedisManager> _redisManager;
	static std::shared_ptr<class SessionManager> _sessionManager;
	static std::shared_ptr<class GameData> _gameData;
	static std::shared_ptr<class GridMap> _gridMap;
	static std::shared_ptr<class World> _world;
};