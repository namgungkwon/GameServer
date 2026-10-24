#include "pch.h"
#include "CoreGlobal.h"

#include "ThreadManager.h"
#include "Memory.h"
#include "SendBuffer.h"
#include "GlobalQueue.h"
#include "JobTimer.h"
#include "DbconnectionPool.h"
#include "Logger.h"

#ifdef _DEBUG
#include "DeadLockProfiler.h"
#endif // _DEBUG

#include "SocketUtils.h"


std::unique_ptr<class Memory>			CoreGlobal::_memory				= nullptr;
std::unique_ptr<class ThreadManager>	CoreGlobal::_threadManager		= nullptr;
std::unique_ptr<class SendBufferManager>CoreGlobal::_sendBufferManager	= nullptr;
std::unique_ptr<class GlobalQueue>		CoreGlobal::_globalQueue		= nullptr;
std::unique_ptr<class JobTimer>			CoreGlobal::_jobTimer			= nullptr;
std::unique_ptr<class DeadLockProfiler>	CoreGlobal::_deadLockProfiler	= nullptr;
std::unique_ptr<class DBConnectionPool>	CoreGlobal::_dbConnectionPool	= nullptr;
std::unique_ptr<class Logger>			CoreGlobal::_logger				= nullptr;


Memory*									GMemory							= nullptr;
ThreadManager*							GThreadManager					= nullptr;
SendBufferManager*						GSendBufferManager				= nullptr;
GlobalQueue*							GGlobalQueue					= nullptr;
JobTimer*								GJobTimer						= nullptr;
DeadLockProfiler*						GDeadLockProfiler				= nullptr;
DBConnectionPool*						GDBConnectionPool				= nullptr;
Logger*									GLogger							= nullptr;



void CoreGlobal::Init()
{
	_memory = std::make_unique<Memory>();
	GMemory = _memory.get();

	SocketUtils::Init();

#ifdef _DEBUG
	_deadLockProfiler			= std::make_unique<DeadLockProfiler>();
	GDeadLockProfiler			= _deadLockProfiler.get();
#endif // _DEBUG

	_threadManager				= std::make_unique<ThreadManager>();
	GThreadManager				= _threadManager.get();

	_sendBufferManager			= std::make_unique<SendBufferManager>();
	GSendBufferManager			= _sendBufferManager.get();

	_globalQueue				= std::make_unique<GlobalQueue>();
	GGlobalQueue				= _globalQueue.get();

	_jobTimer					= std::make_unique<JobTimer>();
	GJobTimer					= _jobTimer.get();

	_dbConnectionPool			= std::make_unique<DBConnectionPool>();
	GDBConnectionPool			= _dbConnectionPool.get();


	_logger						= std::make_unique<Logger>();
	GLogger						= _logger.get();
	GLogger->Init(L"server.log");

	LOG_INFO(L"=== [Server Engine Initialization Complete] ===");
}

void CoreGlobal::Clear()
{
	LOG_INFO(L"=== [Server Engine Shutting Down] ===");

	// Destroy in reverse order: Stats → Logger → Infrastructure → Thread → Memory
	GDBConnectionPool			= nullptr;
	_dbConnectionPool			= nullptr;

	GJobTimer					= nullptr;
	_jobTimer					= nullptr;

	GGlobalQueue				= nullptr;
	_globalQueue				= nullptr;

	GSendBufferManager			= nullptr;
	_sendBufferManager			= nullptr;

	// Logger destructor shuts down the log thread (calls Shutdown())
	GLogger						= nullptr;
	_logger						= nullptr;

	// Wait for all worker threads then release
	if (GThreadManager)
		GThreadManager->Join();
	GThreadManager				= nullptr;
	_threadManager				= nullptr;

	SocketUtils::Clear();

	GMemory						= nullptr;
	_memory						= nullptr;
}