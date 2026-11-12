#include "pch.h"
#include "SessionManager.h"
#include "GameSession.h"

#include "SessionAliveChecker.h"

///////////////////////////////////////////////////////
//	<	SessionManager	>
///////////////////////////////////////////////////////
SessionManager::SessionManager()
{
	_clientChecker = MakeShared<SessionAliveChecker>(
		[this](SessionRef session) { CallBackSessionChecker(session); }, 5000, 1);
	//_clientChecker->Start();
}

GameSessionRef SessionManager::FindSession(uint64 _clientId)
{
	WRITE_LOCK;
	if (_clientSessions.find(_clientId) == _clientSessions.end())
		return nullptr;

	return _clientSessions[_clientId];
}

void SessionManager::Add(GameSessionRef session)
{
	WRITE_LOCK;
	_clientChecker->DoAsync(&SessionAliveChecker::Push, session->GetSessionRef(), session->GetAccountDBId());
	_clientSessions[session->GetAccountDBId()] = std::move(session);
}

void SessionManager::Remove(GameSessionRef session)
{
	WRITE_LOCK;
	_clientSessions.erase(session->GetAccountDBId());
	_clientChecker->DoAsync(&SessionAliveChecker::Pop, session->GetSessionRef(), session->GetAccountDBId());
}

void SessionManager::CallBackSessionChecker(SessionRef session)
{
	return;
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	if (FindSession(gameSession->GetAccountDBId()) == nullptr)
	{
		return;
	}

	gameSession->Disconnect();
}
