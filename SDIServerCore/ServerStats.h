#pragma once

struct NetworkStats
{
	Atomic<uint64> recvBytes = 0;
	Atomic<uint64> sendBytes = 0;
	Atomic<uint64> recvPackets = 0;
	Atomic<uint64> sendPackets = 0;
};

struct JobStats
{
	Atomic<uint64> jobsExecuted = 0;
};

struct IocpStats
{
	Atomic<uint64> iocpCallCount = 0;
	Atomic<uint64> totalProcessTImeUs = 0;
};

struct MemoryStats
{
	Atomic<uint64> poolHitsCount = 0;
	Atomic<uint64> poolMissCount = 0;
	Atomic<int64>  liveAllocCount = 0;
};

struct DBStats
{
	Atomic<uint64> queryCount = 0;
	Atomic<uint64> queryFailed = 0;
	Atomic<uint64> queryTotalUs = 0;
};

struct SystemStats
{
	double cpuUsage = 0.0;
	size_t memUsageMB = 0;
	
	void Refresh();
};

class ServerStats
{
public:
	ServerStats() = default;
	~ServerStats() {};

public:
	void Init();
	void RunUpdateLoop();
	void Shutdown() { _running = false; }

public:
	void Rendering();

public:
	NetworkStats network;
	JobStats job;
	IocpStats iocp;
	MemoryStats memory;
	DBStats DB;
	SystemStats system;

	Atomic<bool> _running = false;
};

