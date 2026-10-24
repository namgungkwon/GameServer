#include "BufferReader.h"


///////////////////////////////////////////////////////
//	<	BufferReader	>
///////////////////////////////////////////////////////
bool BufferReader::Peek(void* dest, const uint32 len)
{
	if (FreeSize() < len)
		return false;

	::memcpy(dest, &_buffer[_pos], len);
	return true;
}

bool BufferReader::Read(void* dest, const uint32 len)
{
	if (Peek(dest, len) == false)
		return false;

	_pos += len;

	return true;
}
