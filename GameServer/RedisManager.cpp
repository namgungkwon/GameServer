#include "pch.h"
#include "RedisManager.h"
#include <optional>
using namespace sw::redis;

bool RedisManager::Init(const RedisConfig& redisConfig, const ServerConfig& serverConfig)
{
	if (Connect(redisConfig) == false)
		return false;


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
		LOG_ERROR(L"Redis Connect Failed");
		return false;
	}
}
