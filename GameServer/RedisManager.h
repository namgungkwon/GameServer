#pragma once
#include "jobQueue.h"
#include <hiredis/hiredis.h>
#include <sw/redis++/redis++.h>

class RedisManager : public JobQueue
{
public:
	bool Init(const RedisConfig& redisConfig, const ServerConfig& serverConfig);


private:
	bool Connect(const RedisConfig& redisConfig);

private:
	unique_ptr<sw::redis::Redis> _redis;
};

