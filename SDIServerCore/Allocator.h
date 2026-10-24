#pragma once


///////////////////////////////////////////////////////
//	<	BaseAllocator	>
//	기본적인 할당자를 오버로딩
///////////////////////////////////////////////////////
class BaseAllocator
{
public:
	static void*		Alloc(const size_t size);
	static void			Release(void* ptr);
};



///////////////////////////////////////////////////////
//	<	StompAllocator	>
//	버그를 잡는데에 사용
//	메모리를 페이지의 끝 부분에 위치시켜서 오버플로우를 감지함
///////////////////////////////////////////////////////
class StompAllocator
{
	enum { PAGE_SIZE = 0x1000 };

public:
	static void*		Alloc(const size_t size);
	static void			Release(void* ptr);
};



///////////////////////////////////////////////////////
//	<	PoolAllocator	>
///////////////////////////////////////////////////////
class PoolAllocator
{
public:
	static void*		Alloc(const size_t size);
	static void			Release(void* ptr);
};



///////////////////////////////////////////////////////
//	<	StlAllocator	>
// 
///////////////////////////////////////////////////////
template<typename T>
class StlAllocator
{
public:
	using value_type = T;

public:
	StlAllocator() {}

	template<typename Other>
	StlAllocator(const StlAllocator<Other>&) {}

public:
	T* allocate(size_t count)
	{
		const size_t size = static_cast<size_t>(count * sizeof(T));
		return static_cast<T*>(PoolAllocator::Alloc(size));
	}

	void deallocate(T* ptr, size_t count)
	{
		PoolAllocator::Release(ptr);
	}

	template<typename U>
	bool operator==(const StlAllocator<U>&) { return true; }

	template<typename U>
	bool operator!=(const StlAllocator<U>&) { return false; }
};



