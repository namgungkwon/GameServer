#pragma once


enum
{
	SLIST_ALIGNMENT = 16
};

///////////////////////////////////////////////////////
//	<	MemoryHeader	>
///////////////////////////////////////////////////////
DECLSPEC_ALIGN(SLIST_ALIGNMENT)
struct MemoryHeader : public SLIST_ENTRY
{
public:
	MemoryHeader(size_t size) : allocSize(size) {}

public:
	// 인자로 받은 헤더에 사이즈를 기입하고 헤더 이후의 주소를 반환
	static void* AttachHeader(MemoryHeader* header, size_t size);
	// 메모리의 주소를 받아서 메모리 앞에 있는 헤더의 주소를 반환
	static MemoryHeader* DetachHeader(void* ptr);

public:
	size_t allocSize;
};



///////////////////////////////////////////////////////
//	<	MemoryPool	>
///////////////////////////////////////////////////////
DECLSPEC_ALIGN(SLIST_ALIGNMENT)
class MemoryPool
{
public:
	MemoryPool(size_t allocSize);
	~MemoryPool();

public:
	void			Push(MemoryHeader* ptr);
	MemoryHeader*	Pop();

private:
	SLIST_HEADER	_header;
	size_t			_allocSize = 0;
	Atomic<int32>	_useCount = 0;
	Atomic<int32>	_reserveCount = 0;
};