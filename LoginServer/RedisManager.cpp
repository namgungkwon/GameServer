#include "pch.h"
#include "RedisManager.h"

using namespace sw::redis;

bool RedisManager::Init(const RedisConfig& redisConfig)
{
	if (Connect(redisConfig) == false)
		return false;
}

std::string RedisManager::SetToken(uint64 accountId)
{
	try
	{
		const string token = GenerateToken();
		_redis->set(token, std::to_string(accountId), std::chrono::seconds(600));
		return token;
	}
	catch (const Error& e) { LOG_ERROR(L"GateRedis IssueAuthToken Failed"); return string(); }
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

string RedisManager::GenerateToken()
{
	static const char charset[] =
		"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	static constexpr std::size_t charset_size = sizeof(charset) - 1; // 62

	std::random_device rd;  // CSPRNG 소스 (구현 의존 but 대부분 안전)
	std::uniform_int_distribution<std::size_t> dist(0, charset_size - 1);

	std::string token;
	token.reserve(TOKEN_LENGTH);
	for (std::size_t i = 0; i < TOKEN_LENGTH; ++i) {
		token += charset[dist(rd)];  // mt19937 없이 바로 사용
	}
	return token;
}
