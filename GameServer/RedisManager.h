#pragma once
#include "jobQueue.h"
#include <hiredis/hiredis.h>
#include <sw/redis++/redis++.h>

class RedisManager : public JobQueue
{
public:
	bool Init(const RedisConfig& redisConfig, const ServerConfig& serverConfig);

public:
	Protocol::Result AuthToken(const string& token, OUT uint64& accountId);

private:
	bool Connect(const RedisConfig& redisConfig);

private:
	std::unique_ptr<sw::redis::Redis> _redis;
};

