#pragma once


///////////////////////////////////////////////////////
//	<	BufferWriter	>
///////////////////////////////////////////////////////
class BufferWriter
{
public:
	BufferWriter() = default;
	~BufferWriter() = default;
	BufferWriter(BYTE* buffer, uint32 size, uint32 pos = 0) : _buffer(buffer), _size(size), _pos(pos) {};

public:
	/* Get & Set */
	BYTE*			Buffer()		{ return _buffer; }
	uint32			Size()			{ return _size; }
	uint32			WriteSize()		{ return _pos; }
	uint32			FreeSize()		{ return _size - _pos; }

public:
	template<typename T>
	bool			Write(T* src)	{ return Write(src, sizeof(T)); }
	bool			Write(const void* src, const uint32 len);

	template<typename T>
	T*				Reserve(size_t count = 1);

	template<typename T>
	BufferWriter&	operator<<(T&& src);

private:
	BYTE*			_buffer = nullptr;
	uint32			_size = 0;
	uint32			_pos = 0;
};



template<typename T>
inline T* BufferWriter::Reserve(size_t count)
{
	const size_t reserveSize = sizeof(T) * count;

	if (FreeSize() < reserveSize)
		return nullptr;

	T* ret = reinterpret_cast<T*>(&_buffer[_pos]);
	_pos += static_cast<uint32>(reserveSize);
	return ret;
}

template<typename T>
inline BufferWriter& BufferWriter::operator<<(T&& src)
{
	using DataType = std::remove_reference_t<T>;
	*reinterpret_cast<DataType*>(&_buffer[_pos]) = std::forward<DataType>(src);
	_pos += sizeof(T);
	return *this;
}
