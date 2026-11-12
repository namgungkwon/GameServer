#pragma once
#include "Types.h"


///////////////////////////////////////////////////////
//	<	Lock	>
//  [WWWWWWWW][WWWWWWWW][RRRRRRRR][RRRRRRRR]
//  W : WriteFlag (Exclusive Lock Owner ThreadId)
//  R : ReadFlag (Shared Lock Count)
///////////////////////////////////////////////////////
class Lock
{
    enum : uint32
    {
        ACQUIRE_TIMEOUT_TICK = 10000,           // 타임 아웃시간
        MAX_SPIN_COUNT = 5000,                  // 스핀 락을 몇 번 돌 것인가?
        WRITE_THREAD_MASK = 0xFFFF'0000,        // 상위 16 비트를 추출하기 위함
        READ_COUNT_MASK = 0x0000'FFFF,          // 하위 16 비트를 추출하기 위함
        EMPTY_FLAG = 0x0000'0000                // 초기 상태를 부여
    };

public:
    void                    WriteLock(const char* name);
    void                    WriteUnlock(const char* name);
    void                    ReadLock(const char* name);
    void                    ReadUnlock(const char* name);

private:
    Atomic<uint32>          _lockFlag = EMPTY_FLAG;
    uint16                  _writeCount = 0;
};



///////////////////////////////////////////////////////
//	<	ReadLockGuard	>
///////////////////////////////////////////////////////
class ReadLockGuard
{
public:
    ReadLockGuard(Lock& lock, const char* name) : _lock(lock), _name(name) { _lock.ReadLock(name); }
    ~ReadLockGuard() { _lock.ReadUnlock(_name); }

private:
    Lock&                   _lock;
    const char*             _name;
};



///////////////////////////////////////////////////////
//	<	WriteLockGuard	>
///////////////////////////////////////////////////////
class WriteLockGuard
{
public:
    WriteLockGuard(Lock& lock, const char* name) : _lock(lock), _name(name) { _lock.WriteLock(name); }
    ~WriteLockGuard() { _lock.WriteUnlock(_name); }

private:
    Lock&                   _lock;
    const char*             _name;
};