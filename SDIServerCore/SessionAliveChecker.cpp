#include "pch.h"
#include "SessionAliveChecker.h"


///////////////////////////////////////////////////////
//	<	SessionAliveChecker	>
///////////////////////////////////////////////////////
SessionAliveChecker::SessionAliveChecker(CheckedSessionCallBack callBackFunc, uint32 timeOutMs, uint32 checkTimes)
{
	_callBackFunc = callBackFunc;
	_sessions.resize(LIST_SIZE);

	_currentIdx = 0;
	_timeOutMs = timeOutMs;
	_checkInterval = (timeOutMs / checkTimes) / LIST_SIZE;
}

void SessionAliveChecker::Push(SessionRef session, uint64 uniqueId)
{
	const uint64 idx = (uniqueId + 1) % LIST_SIZE;
	_sessions[idx].push_back(session);
}

void SessionAliveChecker::Pop(SessionRef session, uint64 uniqueId)
{
	const uint64 idx = (uniqueId + 1) % LIST_SIZE;
	
	for (auto it = _sessions[idx].begin(); it != _sessions[idx].end(); ++it)
	{
		if (*it == session)
		{
			_sessions[idx].erase(it);
			break;
		}
	}
}

void SessionAliveChecker::Start()
{
	_start = true;
	DoTimer(_checkInterval, &SessionAliveChecker::AliveCheck);
}

void SessionAliveChecker::AliveCheck()
{
	const uint64 admitTickLimit = ::GetTickCount64() - _timeOutMs;

	for (auto it = _sessions[_currentIdx].begin(); it != _sessions[_currentIdx].end();)
	{
		SessionRef session = *it;
		if (session->GetLastReceivedTime() < admitTickLimit)
		{
			_callBackFunc(session);
			it = _sessions[_currentIdx].erase(it);
		}
		else
		{
			++it;
		}
	}
	_currentIdx = (_currentIdx + 1) % LIST_SIZE;

	if (_start)
		DoTimer(_checkInterval, &SessionAliveChecker::AliveCheck);
}