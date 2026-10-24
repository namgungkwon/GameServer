#include "pch.h"
#include "Allocator.h"
#include "Memory.h"


///////////////////////////////////////////////////////
//	<	BaseAllocator	>
///////////////////////////////////////////////////////
void* BaseAllocator::Alloc(const size_t size)
{
	return ::malloc(size);
}

void BaseAllocator::Release(void* ptr)
{
	::free(ptr);
}



///////////////////////////////////////////////////////
//	<	StompAllocator	>
///////////////////////////////////////////////////////
void* StompAllocator::Alloc(const size_t size)
{
	const size_t pageCount = (size + PAGE_SIZE - 1) / PAGE_SIZE;
	const size_t dataOffset = pageCount * PAGE_SIZE - size;
	void* baseAddress = ::VirtualAlloc(NULL, pageCount * PAGE_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	return static_cast<void*>(static_cast<int8*>(baseAddress) + dataOffset);
}

void StompAllocator::Release(void* ptr)
{
	const size_t address = reinterpret_cast<size_t>(ptr);
	const size_t baseAddress = address - (address % PAGE_SIZE);

	::VirtualFree(reinterpret_cast<void*>(baseAddress), 0, MEM_RELEASE);
}



///////////////////////////////////////////////////////
//	<	PoolAllocator	>
///////////////////////////////////////////////////////
void* PoolAllocator::Alloc(const size_t size)
{
	return GMemory->Allocate(size);
}

void PoolAllocator::Release(void* ptr)
{
	GMemory->Release(ptr);
}