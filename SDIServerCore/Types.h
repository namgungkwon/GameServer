#pragma once
#include <mutex>
#include <atomic>



///////////////////////////////////////////////////////
//	<	Types	>
///////////////////////////////////////////////////////
using BYTE				= unsigned char;
using int8				= __int8;
using int16				= __int16;
using int32				= __int32;
using int64				= __int64;
using uint8				= unsigned __int8;
using uint16			= unsigned __int16;
using uint32			= unsigned __int32;
using uint64			= unsigned __int64;



///////////////////////////////////////////////////////
//	<	Lock	>
///////////////////////////////////////////////////////
template<typename T>
using Atomic			= std::atomic<T>;
using CondVar			= std::condition_variable;
using Mutex				= std::mutex;
using UniqueLock		= std::unique_lock<Mutex>;
using LockGuard			= std::lock_guard<Mutex>;



///////////////////////////////////////////////////////
//	<	shared_ptr	>
///////////////////////////////////////////////////////
#define useRef(className) using className##Ref = std::shared_ptr<class className>;
useRef(IocpObject)
useRef(IocpCore)
useRef(Session)
useRef(PacketSession)
useRef(Listener)
useRef(Service)
useRef(ServerService)
useRef(ClientService)
useRef(SendBuffer)
useRef(SendBufferChunk)
useRef(Job)
useRef(JobQueue)
useRef(SessionAliveChecker)