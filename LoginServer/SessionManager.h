#pragma once

///////////////////////////////////////////////////////
//	<	ClientSessionManager	>
///////////////////////////////////////////////////////
class SessionManager
{
	friend class ClientSession;
public:
	SessionManager();
public:
	GameSessionRef					FindSession(uint64 _clientId);

private:
	void							Add(GameSessionRef session);
	void							Remove(GameSessionRef session);

private:
	void							CallBackSessionChecker(SessionRef session);

private:
	USE_LOCK;
	SessionAliveCheckerRef			_clientChecker;
	Map<uint64, GameSessionRef>		_clientSessions;
};