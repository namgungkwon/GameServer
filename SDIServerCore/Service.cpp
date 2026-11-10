#include "pch.h"
#include "Service.h"
#include "Session.h"
#include "Listener.h"

#include "ThreadManager.h"


///////////////////////////////////////////////////////
//	<	Service	>
///////////////////////////////////////////////////////
Service::Service(ServiceType type, NetAddress address, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount)
	: _type(type), _netAddress(address), _iocpCore(core), _sessionFactory(factory), _maxSessionCount(maxSessionCount)
{

}

Service::~Service()
{
	CloseService();
}

bool Service::Process(uint32 iocpTimeoutMs)
{
	if (_isClose) return false;
	return GetIocpCore()->Dispatch(iocpTimeoutMs);
}

void Service::CloseService()
{
	_isClose = true;

	WRITE_LOCK;
	for (const auto& session : _sessions)
	{
		session->Disconnect(L"Close Service");
	}
	_sessions.clear();
}

void Service::Broadcast(SendBufferRef& sendBuffer)
{
	WRITE_LOCK;
	for (const auto& session : _sessions)
	{
		session->Send(sendBuffer);
	}
}

SessionRef Service::CreateSession()
{
	SessionRef session = _sessionFactory();
	session->SetService(shared_from_this());

	if (_iocpCore->Register(session) == false)
		return nullptr;

	return session;
}

bool Service::AddSession(SessionRef session)
{
	WRITE_LOCK;
	_sessionCount++;
	_sessions.insert(session);

	return _sessionCount <= _maxSessionCount;
}

void Service::ReleaseSession(SessionRef session)
{
	WRITE_LOCK;
	ASSERT_CRASH(_sessions.erase(session) != 0);
	_sessionCount--;
}



///////////////////////////////////////////////////////
//	<	ClientService	>
///////////////////////////////////////////////////////
ClientService::ClientService(NetAddress targetAddress, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount)
	: Service(ServiceType::Client, targetAddress, core, factory, maxSessionCount)
{

}

bool ClientService::Start()
{
	if (CanStart() == false)
		return false;

	return Service::Start();
}

SessionRef ClientService::AddSession()
{
	SessionRef session = CreateSession();
	session->Connect();
	return session;
}



///////////////////////////////////////////////////////
//	<	ServerService	>
///////////////////////////////////////////////////////
ServerService::ServerService(NetAddress address, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount, int32 acceptPoolCount)
	: Service(ServiceType::Server, address, core, factory, maxSessionCount)
{
	_acceptPoolCount = static_cast<size_t>(acceptPoolCount);
}

bool ServerService::Start()
{
	if (CanStart() == false)
		return false;

	_listener = MakeShared<Listener>();
	if (_listener == nullptr)
		return false;

	ServerServiceRef service = static_pointer_cast<ServerService>(shared_from_this());
	if (_listener->StartAccept(service) == false)
		return false;

	return Service::Start();
}

void ServerService::CloseService()
{
	// TODO
	Service::CloseService();

	_listener = nullptr;
}
