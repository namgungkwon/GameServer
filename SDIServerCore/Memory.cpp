#include "pch.h"
#include "Memory.h"

#include "MemoryPool.h"


///////////////////////////////////////////////////////
//	<	Memory	>
///////////////////////////////////////////////////////
Memory::Memory()
{
#ifndef _STOMP
	_pools.reserve(static_cast<size_t>(POOL_COUNT));

	size_t currentSize = 0;
	size_t tableIndex = 0;
	size_t poolIndex = 0;

	for (size_t size = currentSize + 32; size <= 1024; size += 32)
	{
		MemoryPool* pool = new MemoryPool(size);
		_pools[poolIndex] = pool;
		poolIndex++;

		while (tableIndex <= size)
		{
			_poolTable[tableIndex] = pool;
			tableIndex++;
		}
		currentSize = size;
	}

	for (size_t size = currentSize + 128; size <= 2048; size += 128)
	{
		MemoryPool* pool = new MemoryPool(size);
		_pools[poolIndex] = pool;
		poolIndex++;

		while (tableIndex <= size)
		{
			_poolTable[tableIndex] = pool;
			tableIndex++;
		}
		currentSize = size;
	}

	for (size_t size = currentSize + 256; size <= 4096; size += 256)
	{
		MemoryPool* pool = new MemoryPool(size);
		_pools[poolIndex] = pool;
		poolIndex++;

		while (tableIndex <= size)
		{
			_poolTable[tableIndex] = pool;
			tableIndex++;
		}
		currentSize = size;
	}

	for (size_t size = currentSize + 512; size <= 8192; size += 512)
	{
		MemoryPool* pool = new MemoryPool(size);
		_pools[poolIndex] = pool;
		poolIndex++;

		while (tableIndex <= size)
		{
			_poolTable[tableIndex] = pool;
			tableIndex++;
		}
		currentSize = size;
	}
#endif // _STOMP
}

Memory::~Memory()
{
	for (MemoryPool* pool : _pools)
		delete pool;

	_pools.clear();
}

#pragma optimize("", off)
void* Memory::Allocate(size_t size)
{
	MemoryHeader* header = nullptr;
	const size_t allocSize = size + sizeof(MemoryHeader);

#ifdef _STOMP
	header = reinterpret_cast<MemoryHeader*>(StompAllocator::Alloc(allocSize));
#else
	if (allocSize > MAX_ALLOC_SIZE)
	{
		// 메모리 풀링 최대 크기를 벗어나면 일반 할당
		header = reinterpret_cast<MemoryHeader*>(::_aligned_malloc(allocSize, SLIST_ALIGNMENT));
		if (GServerStats) GServerStats->memory.poolMissCount.fetch_add(1);
	}
	else
	{
		// 메모리 풀에서 꺼내온다
		header = _poolTable[allocSize]->Pop();
		if (GServerStats) GServerStats->memory.poolHitsCount.fetch_add(1);
	}

	if (GServerStats) GServerStats->memory.liveAllocCount.fetch_add(1);
#endif	//_STOMP

	return MemoryHeader::AttachHeader(header, allocSize);
}
#pragma optimize("", on)

void Memory::Release(void* ptr)
{
	MemoryHeader* header = MemoryHeader::DetachHeader(ptr);

	const size_t allocSize = header->allocSize;
	ASSERT_CRASH(allocSize > 0);

#ifdef _STOMP
	StompAllocator::Release(header);
#else
	if (allocSize > MAX_ALLOC_SIZE)
	{
		// 메모리 풀링 최대 크기를 벗어나면 일반 해제
		::_aligned_free(header);
	}
	else
	{
		// 메모리 풀에 반납한다
		_poolTable[allocSize]->Push(header);
	}
	if (GServerStats) GServerStats->memory.liveAllocCount.fetch_sub(1);
#endif	//_STOMP
}
