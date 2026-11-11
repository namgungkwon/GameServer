#include "pch.h"
#include "PacketSender.h"

void PacketSender::Init(size_t countOfSender)
{
	_size = countOfSender;
	_senders.reserve(countOfSender);
	for (size_t i = 0; i < countOfSender; ++i)
	{
		_senders.push_back(MakeShared<JobQueue>());
	}
}

JobQueueRef PacketSender::GetSender(uint64 uniqueId)
{
	return _senders[uniqueId % _size];
}