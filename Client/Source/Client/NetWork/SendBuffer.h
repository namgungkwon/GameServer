#pragma once



class SendBuffer : public TSharedFromThis<SendBuffer>
{
	
public :
	SendBuffer(int32 bufferSize) { _buffer.SetNum(bufferSize);};
	~SendBuffer() {};

public:
	BYTE* Buffer() { return _buffer.GetData();}
	int32 WriteSize() { return _writeSize;}
	int32 Capacity() { return static_cast<int32>(_buffer.Num());}

	void CopyData(void* data, int32 len) { memcpy(_buffer.GetData(), data, len); _writeSize = len;}
	void Close(uint32 writeSize) { _writeSize = writeSize;}

private:
	TArray<BYTE> _buffer;
	int32 _writeSize;
};
