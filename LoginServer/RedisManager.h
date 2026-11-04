#pragma once
#include "jobQueue.h"
#include <hiredis/hiredis.h>
#include <sw/redis++/redis++.h>

enum
{
	TOKEN_LENGTH = 32,
	TOKEN_EXPIRED_SECONDS = 1800,
};

class RedisManager : public JobQueue
{
public:
	bool Init(const RedisConfig& redisConfig);

public:
	std::string SetToken(uint64 accountId);

private:
	bool Connect(const RedisConfig& redisConfig);
	string GenerateToken();

private:
	std::unique_ptr<sw::redis::Redis> _redis;
};

