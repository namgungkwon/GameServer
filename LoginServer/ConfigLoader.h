#pragma once

struct DBConfig
{
	int32 trustedConnection = 0;
	wstring host = L"127.0.0.1";
	uint32 port = 8001;
	wstring user = L"root";
	wstring password;
	wstring name = L"portfolio";
	int32  threads = 1;
};

struct RedisConfig
{
	wstring host = L"127.0.0.1";
	int32  port = 6379;
};

struct ServerConfig
{
	int32 id = 1;
	wstring name = L"Server1";
	wstring ip = L"127.0.0.1";
	uint32 port = 8000;
	int32 maxSessions = 3000;
	int32 acceptPool = 10;
};

struct WorldConfig
{
	int32 mapSize = 1000;
	int32 zoneSize = 20;
	int32 sceneCount = 8;
};



struct GameConfig
{
	ServerConfig gameServer;
	ServerConfig loginServer;
	DBConfig db;
	RedisConfig redis;
	WorldConfig world;
};

class ConfigLoader
{
public:
	static bool Load(const string& path, OUT GameConfig& config);
};