#include "pch.h"
#include "CoreGlobal.h"

#include "ThreadManager.h"
#include "Memory.h"
#include "SendBuffer.h"
#include "GlobalQueue.h"
#include "JobTimer.h"
#include "DbconnectionPool.h"
#include "Logger.h"
#include "ServerStats.h"
#include "Profiler.h"

#ifdef _DEBUG
#include "DeadLockProfiler.h"
#endif // _DEBUG

#include "SocketUtils.h"

#ifdef _DEBUG
std::unique_ptr<class DeadLockProfiler>	CoreGlobal::_deadLockProfiler	= nullptr;
#endif // _DEBUG

std::unique_ptr<class Memory>			CoreGlobal::_memory				= nullptr;
std::unique_ptr<class ThreadManager>	CoreGlobal::_threadManager		= nullptr;
std::unique_ptr<class SendBufferManager>CoreGlobal::_sendBufferManager	= nullptr;
std::unique_ptr<class GlobalQueue>		CoreGlobal::_globalQueue		= nullptr;
std::unique_ptr<class JobTimer>			CoreGlobal::_jobTimer			= nullptr;
std::unique_ptr<class DBConnectionPool>	CoreGlobal::_dbConnectionPool	= nullptr;
std::unique_ptr<class Logger>			CoreGlobal::_logger				= nullptr;
std::unique_ptr<class ServerStats>		CoreGlobal::_serverStats		= nullptr;
std::unique_ptr<class Profiler>			CoreGlobal::_profiler			= nullptr;

Memory*									GMemory							= nullptr;
ThreadManager*							GThreadManager					= nullptr;
SendBufferManager*						GSendBufferManager				= nullptr;
GlobalQueue*							GGlobalQueue					= nullptr;
JobTimer*								GJobTimer						= nullptr;
DeadLockProfiler*						GDeadLockProfiler				= nullptr;
DBConnectionPool*						GDBConnectionPool				= nullptr;
Logger*									GLogger							= nullptr;
ServerStats*							GServerStats					= nullptr;
Profiler*								GProfiler						= nullptr;


void CoreGlobal::Init()
{
	// 1. Memory
	_memory = std::make_unique<Memory>();
	GMemory = _memory.get();

	// 2. SocketUtils
	SocketUtils::Init();

	// 3. DeadLockProfiler(Debug)
#ifdef _DEBUG
	_deadLockProfiler			= std::make_unique<DeadLockProfiler>();
	GDeadLockProfiler			= _deadLockProfiler.get();
#endif // _DEBUG

	// 4. ThreadManager
	_threadManager = std::make_unique<ThreadManager>();
	GThreadManager = _threadManager.get();

	// 5. Logger(Use ThreadManager)
	_logger = std::make_unique<Logger>();
	GLogger = _logger.get();
	GLogger->Init(L"server.log");

	// 6. Profiler
	_profiler = std::make_unique<Profiler>();
	GProfiler = _profiler.get();
	GProfiler->Init(L"");
	
	// 7. SendBufferManager
	_sendBufferManager			= std::make_unique<SendBufferManager>();
	GSendBufferManager			= _sendBufferManager.get();

	// 8. GlobalQueue
	_globalQueue				= std::make_unique<GlobalQueue>();
	GGlobalQueue				= _globalQueue.get();
	GGlobalQueue->Start();

	// 9. JobTimer
	_jobTimer					= std::make_unique<JobTimer>();
	GJobTimer					= _jobTimer.get();
	GJobTimer->Start();

	// 10. DBConnectionPool
	_dbConnectionPool			= std::make_unique<DBConnectionPool>();
	GDBConnectionPool			= _dbConnectionPool.get();

	// 11. ServerStat
	_serverStats = std::make_unique<ServerStats>();
	GServerStats = _serverStats.get();
	GServerStats->Init();

	LOG_INFO(L"=== [Core Initialization Complete] ===");
}

void CoreGlobal::Clear()
{
	LOG_INFO(L"=== [Server Engine Shutting Down] ===");

	// 1. ServerStat
	_serverStats = nullptr;
	GServerStats = nullptr;
	
	// 2. DBConnectionPool
	GDBConnectionPool			= nullptr;
	_dbConnectionPool			= nullptr;

	// 3. JobTimer
	if (GJobTimer)
		GJobTimer->Stop();
	GJobTimer					= nullptr;
	_jobTimer					= nullptr;

	// 4. GlobalQueue
	if (GGlobalQueue)
		GGlobalQueue->Stop();
	GGlobalQueue				= nullptr;
	_globalQueue				= nullptr;

	// 5. SendBufferManager
	GSendBufferManager			= nullptr;
	_sendBufferManager			= nullptr;

	// 6. Profiler
	GProfiler					= nullptr;
	_profiler					= nullptr;

	// 6. Logger
	if (GLogger)
		GLogger->Shutdown();
	GLogger						= nullptr;
	_logger						= nullptr;

	// 7. ThreadManager
	if (GThreadManager)
		GThreadManager->Join();
	GThreadManager				= nullptr;
	_threadManager				= nullptr;

	// 8. DeadLockProfiler(Debug)
#ifdef _DEBUG
	GDeadLockProfiler			= nullptr;
	_deadLockProfiler			= nullptr;
#endif // _DEBUG

	// 9. SocketUtils
	SocketUtils::Clear();

	// 10. Memory
	GMemory						= nullptr;
	_memory						= nullptr;
}