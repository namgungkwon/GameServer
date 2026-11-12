#include "pch.h"
#include "MemoryPool.h"


///////////////////////////////////////////////////////
//	<	MemoryHeader	>
///////////////////////////////////////////////////////
void* MemoryHeader::AttachHeader(MemoryHeader* header, size_t size)
{
	new(header)MemoryHeader(size); // placement new
	return reinterpret_cast<void*>(++header);
}

MemoryHeader* MemoryHeader::DetachHeader(void* ptr)
{
	return reinterpret_cast<MemoryHeader*>(ptr) - 1;
}



///////////////////////////////////////////////////////
//	<	MemoryPool	>
///////////////////////////////////////////////////////
MemoryPool::MemoryPool(size_t allocSize) : _allocSize(allocSize)
{
	::InitializeSListHead(&_header);
}

MemoryPool::~MemoryPool()
{
	while (MemoryHeader* memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&_header)))
	{
		::_aligned_free(memory);
	}
}

void MemoryPool::Push(MemoryHeader * ptr)
{
	ptr->allocSize = 0;

	::InterlockedPushEntrySList(&_header, static_cast<PSLIST_ENTRY>(ptr));

	_useCount.fetch_sub(1);
	_reserveCount.fetch_add(1);
}

MemoryHeader* MemoryPool::Pop()
{
	MemoryHeader* memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&_header));

	// 없으면 새로 만들다
	if (memory == nullptr)
	{
		memory = reinterpret_cast<MemoryHeader*>(::_aligned_malloc(_allocSize, SLIST_ALIGNMENT));
	}
	else
	{
		ASSERT_CRASH(memory->allocSize == 0);
		_reserveCount.fetch_sub(1);
	}

	_useCount.fetch_add(1);

	return memory;
}
