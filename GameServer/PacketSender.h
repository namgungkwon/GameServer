#pragma once

#include "jobQueue.h"


///////////////////////////////////////////////////////
//	<	PacketSender	>
///////////////////////////////////////////////////////
class PacketSender : public JobQueue
{
public:
	void					Init(size_t countOfSender);

	JobQueueRef				GetSender(uint64 uniqueId);

private:
	Vector<JobQueueRef>		_senders;
	uint64					_size = 0;
};