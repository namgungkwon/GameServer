#pragma once

#include "Session.h"

#include "JobQueue.h"

enum
{
	LIST_SIZE = 16,
};

using CheckedSessionCallBack = function<void(SessionRef)>;


///////////////////////////////////////////////////////
//	<	SessionAliveChecker	>
///////////////////////////////////////////////////////
class SessionAliveChecker : public JobQueue
{
public:
	SessionAliveChecker(CheckedSessionCallBack callBackFunc, uint32 timeOutMs = 30000, uint32 checkTimes = 3);

public:
	void						Start();
	void						Stop() { _start = false; }
	void						Push(SessionRef session, uint64 uniqueId);
	void						Pop(SessionRef session, uint64 uniqueId);

private:
	void						AliveCheck();

private:
	bool						_start = false;

	Vector<List<SessionRef>>	_sessions;

	uint64						_timeOutMs;
	uint64						_checkInterval;
	uint32						_currentIdx;

	CheckedSessionCallBack		_callBackFunc;
};