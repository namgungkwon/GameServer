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
#define useRef_Class(className) using className##Ref = std::shared_ptr<class className>;
#define useRef_Struct(structName) using structName##Ref = std::shared_ptr<struct structName>;
useRef_Class(IocpObject)
useRef_Class(IocpCore)
useRef_Class(Session)
useRef_Class(PacketSession)
useRef_Class(Listener)
useRef_Class(Service)
useRef_Class(ServerService)
useRef_Class(ClientService)
useRef_Class(SendBuffer)
useRef_Class(SendBufferChunk)
useRef_Class(Job)
useRef_Class(JobQueue)
useRef_Class(SessionAliveChecker)