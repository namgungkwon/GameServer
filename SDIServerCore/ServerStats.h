#pragma once

struct NetworkStats
{
	Atomic<int32>  connectedSessions = 0;		// PacketSession::OnConnected(); PacketSession::OnDisconnected();
	Atomic<uint64> totalConnections = 0;		// PacketSession::OnConnected();
	Atomic<uint64> recvBytes = 0;				// Session::ProcessRecv();
	Atomic<uint64> sendBytes = 0;				// Session::ProcessSend();
	Atomic<uint64> recvPackets = 0;				// Session::OnRecv();
	Atomic<uint64> sendPackets = 0;				// Session::ProcessSend();
};

struct JobStats
{
	Atomic<uint64> jobsExecuted = 0;			// JobQueue::Execute();
};

struct IocpStats
{
	Atomic<uint64> iocpCallCount = 0;			// IocpCore::Dispatch();
	Atomic<uint64> totalProcessTImeUs = 0;		// IocpCore::Dispatch();
};

struct MemoryStats
{
	Atomic<uint64> poolHitsCount = 0;			// Memory::Allocate();
	Atomic<uint64> poolMissCount = 0;			// Memory::Allocate();
	Atomic<int64>  liveAllocCount = 0;			// Memory::Allocate(); Memory::Release();
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

struct GameStats
{
	Atomic<int32>  activePlayers = 0;
	Atomic<uint64> tickCount = 0;
	Atomic<uint64> lagTickCount = 0;
	Atomic<uint64> lagTickMs = 0;
	Atomic<uint64> lagTickMaxMs = 0;
};


///////////////////////////////////////////////////////
//	<	ServerStats	>
///////////////////////////////////////////////////////
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
	GameStats game;

	Atomic<bool> _running = false;
};

