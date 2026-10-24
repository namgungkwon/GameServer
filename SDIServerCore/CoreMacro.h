#pragma once

#define OUT


///////////////////////////////////////////////////////
//	<	Namespace	>
///////////////////////////////////////////////////////
#define NAMESPACE_BEGIN(name)	namespace name {
#define NAMESPACE_END			}

///////////////////////////////////////////////////////
//	<	Lock	>
///////////////////////////////////////////////////////
#define USE_MANY_LOCKS(count)	Lock _locks[count];
#define USE_LOCK				USE_MANY_LOCKS(1)
#define	READ_LOCK_IDX(idx)		ReadLockGuard readLockGuard_##idx(_locks[idx], typeid(this).name());
#define READ_LOCK				READ_LOCK_IDX(0)
#define	WRITE_LOCK_IDX(idx)		WriteLockGuard writeLockGuard_##idx(_locks[idx], typeid(this).name());
#define WRITE_LOCK				WRITE_LOCK_IDX(0)



///////////////////////////////////////////////////////
//	<	Memory	>
///////////////////////////////////////////////////////
#define calloc(size)	PoolAllocator::Alloc(size)
#define crelease(ptr)	PoolAllocator::Release(ptr)

#ifdef _DEBUG
#define _STOMP
#else
#endif // _DEBUG



///////////////////////////////////////////////////////
//	<	Crash	>
///////////////////////////////////////////////////////
#define CRASH(cause)						\
{											\
	uint32* crash = nullptr;				\
	__analysis_assume(crash != nullptr);	\
	*crash = 0xDEADBEEF;					\
}

// 조건부로 크래시를 내고 싶을 떄
#define ASSERT_CRASH(expr)			\
{									\
	if (!(expr))					\
	{								\
		CRASH("ASSERT_CRASH");		\
		__analysis_assume(expr);	\
	}								\
}


///////////////////////////////////////////////////////
//	<	size	>
// len - 배열의 길이 반환
// size - 배열의 크기 반환
///////////////////////////////////////////////////////
#define size16(val)		static_cast<int16>(sizeof(val))
#define size32(val)		static_cast<int32>(sizeof(val))
#define len16(arr)		static_cast<int16>(sizeof(arr) / sizeof(arr[0]))
#define len32(arr)		static_cast<int32>(sizeof(arr) / sizeof(arr[0]))