#include "pch.h"
#include "BufferWriter.h"


///////////////////////////////////////////////////////
//	<	BufferWriter	>
///////////////////////////////////////////////////////
bool BufferWriter::Write(const void* src, const uint32 len)
{
	if (FreeSize() < len)
		return false;

	::memcpy(&_buffer[_pos], src, static_cast<size_t>(len));
	_pos += len;

	return true;
}