#include "pch.h"
#include "RedisManager.h"
using namespace sw::redis;


///////////////////////////////////////////////////////
//	<	RedisManager	>
///////////////////////////////////////////////////////
bool RedisManager::Init(const RedisConfig& redisConfig, const ServerConfig& serverConfig)
{
	if (Connect(redisConfig) == false)
		return false;
	return true;
}

Protocol::Result RedisManager::AuthToken(const string& token, OUT uint64& accountId)
{
	try
	{
		auto val = _redis->get(token);
		if (!val) return Protocol::TOKEN_INVALID;

		accountId = std::stoull(*val);
		return Protocol::SUCCESS;
	}
	catch (const Error& e)
	{
		LOG_ERROR(L"Redis GetAccountId Failed: " + Utils::ToWString(e.what()));
		return Protocol::REDIS_ERROR;
	}
}

bool RedisManager::Connect(const RedisConfig& redisConfig)
{
	try
	{
		ConnectionOptions options;
		options.host = Utils::ToString(redisConfig.host);
		options.port = redisConfig.port;
		options.socket_timeout = std::chrono::milliseconds(1000);

		ConnectionPoolOptions poolOptions;
		poolOptions.size = 1;

		_redis = std::make_unique<Redis>(options, poolOptions);
		_redis->ping();
		return true;
	}
	catch (const Error& e)
	{
		LOG_ERROR(L"Redis Connect Failed: " + Utils::ToWString(e.what()));
		return false;
	}
}
