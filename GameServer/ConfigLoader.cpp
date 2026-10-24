#include "pch.h"
#include "ConfigLoader.h"

#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

template<typename T>
void ReadField(const json& j, const char* key, T& out)
{
	if (j.contains(key)) out = j[key].get<T>();
}

void ReadWString(const json& j, const char* key, wstring& out)
{
	if (j.contains(key)) out = Utils::ToWString(j[key].get<std::string>());
}


bool ConfigLoader::Load(const string& path, GameConfig& config)
{
	std::ifstream file(path);
	if (file.is_open() == false)
	{
		LOG_ERROR(L"Config file not found");
		return false;
	}

	json j;
	try
	{
		file >> j;
	}
	catch (const json::parse_error& e)
	{
		LOG_ERROR(L"Config parse error: " + Utils::ToWString(e.what()));
	}

	if (j.contains("database"))
	{
		auto& db = j["database"];
		ReadField(db, "trustedConnection", config.db.trustedConnection);
		ReadWString(db, "host", config.db.host);
		ReadField(db, "port", config.db.port);
		ReadWString(db, "user", config.db.user);
		ReadWString(db, "password", config.db.password);
		ReadWString(db, "name", config.db.name);
		ReadField(db, "threads", config.db.threads);
	}

	if (j.contains("redis"))
	{
		auto& r = j["redis"];
		ReadWString(r, "host", config.redis.host);
		ReadField(r, "port", config.redis.port);
	}

	if (j.contains("server"))
	{
		auto& server = j["server"];
		ReadField(server, "id", config.server.id);
		ReadWString(server, "name", config.server.name);
		ReadWString(server, "ip", config.server.ip);
		ReadField(server, "port", config.server.port);
		ReadField(server, "masSessions", config.server.maxSessions);
		ReadField(server, "acceptPool", config.server.acceptPool);
	}

	if (j.contains("world"))
	{
		auto& server = j["world"];
		ReadField(server, "mapSize", config.world.mapSize);
		ReadField(server, "zoneSize", config.world.zoneSize);
		ReadField(server, "sceneCount", config.world.sceneCount);
	}
	return true;
}
