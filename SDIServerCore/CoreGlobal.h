#pragma once

extern class ThreadManager*		GThreadManager;
extern class Memory*			GMemory;
extern class SendBufferManager* GSendBufferManager;
extern class GlobalQueue*		GGlobalQueue;
extern class JobTimer*			GJobTimer;

extern class DeadLockProfiler*	GDeadLockProfiler;
extern class DBConnectionPool*	GDBConnectionPool;
extern class Logger*			GLogger;

class CoreGlobal
{
public:
	static void Init();
	static void Clear();

private:
	static std::unique_ptr<class Memory> _memory;
	static std::unique_ptr<class ThreadManager> _threadManager;
	static std::unique_ptr<class SendBufferManager> _sendBufferManager;
	static std::unique_ptr<class GlobalQueue> _globalQueue;
	static std::unique_ptr<class JobTimer>	_jobTimer;
	static std::unique_ptr<class DeadLockProfiler> _deadLockProfiler;
	static std::unique_ptr<class DBConnectionPool> _dbConnectionPool;
	static std::unique_ptr<class Logger> _logger;
};