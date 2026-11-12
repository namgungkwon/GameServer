#pragma once

/* 전방 선언 */
class MemoryPool;


///////////////////////////////////////////////////////
//	<	Memory	>
///////////////////////////////////////////////////////
class Memory
{
	enum
	{
		// ~1024까지 32단위, ~2048까지 128단위, ~4096까지 256단위
		POOL_COUNT = (1024 / 32) + (1024 / 128) + (2048 / 256) + (4096 / 512),
		MAX_ALLOC_SIZE = 8192
	};

public:
	Memory();
	~Memory();

public:
	void*					Allocate(size_t size);
	void					Release(void* ptr);

private:
	vector<MemoryPool*>		_pools;		// 실제 풀은 enum에서 쓰인 단위별로 만들게 되고,
	MemoryPool*				_poolTable[MAX_ALLOC_SIZE + 1]; // 원하는 메모리용량에 맞는 풀을 가리키게 만든다.
};



///////////////////////////////////////////////////////
//	<	Allocate cnew & cdelete	>
// 	ObjectPool 이용시
//		-ObjectPool<Type>::MakeShared();
//	MemoryPool 이용시
//		- MakeShared<Type>();
///////////////////////////////////////////////////////
template<typename Type, typename... Args>
Type* cnew(Args&&... args)
{
	Type* memory = static_cast<Type*>(PoolAllocator::Alloc(sizeof(Type)));

	// placement new
	new(memory)Type(std::forward<Args>(args)...);
	return memory;
}

template<typename Type>
void cdelete(Type* obj)
{
	obj->~Type();
	PoolAllocator::Release(obj);
}

template<typename Type, typename... Args>
shared_ptr<Type> MakeShared(Args&&... args)
{
	return shared_ptr<Type>{ cnew<Type>(std::forward<Args>(args)...), cdelete<Type>};
}