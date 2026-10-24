#pragma once
#include "NetAddress.h"
#include "IocpCore.h"
#include "Listener.h"
#include <functional>

enum class ServiceType : uint8
{
	Server,
	Client,
};

using SessionFactory = function<SessionRef(void)>;


///////////////////////////////////////////////////////
//	<	Service	>
///////////////////////////////////////////////////////
class Service : public enable_shared_from_this<Service>
{
	friend class Listener;
	friend class Session;

public:
	Service(ServiceType type, NetAddress address, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount = 1);
	virtual ~Service()					= default;

public:
	virtual bool				Start() abstract						{ return false; };
	bool						CanStart()								{ return _sessionFactory != nullptr; }
	virtual bool				Process(uint32 iocpTimeoutMs = INFINITE);
	virtual void				CloseService();

public:
	void						Broadcast(SendBufferRef& sendBuffer);

public:
	void						SetSessionFactory(SessionFactory func)	{ _sessionFactory = func; }

	int32						GetCurrentSessionCount()				{ return _sessionCount; }
	int32						GetMaxSessionCount()					{ return _maxSessionCount; }
	ServiceType					GetServiceType()						{ return _type; }
	NetAddress					GetNetAddress()							{ return _netAddress; }

protected:
	SessionRef					CreateSession();
	bool						AddSession(SessionRef session);
	void						ReleaseSession(SessionRef session);

	IocpCoreRef&				GetIocpCore()							{ return _iocpCore; }

protected:
	USE_LOCK;

	ServiceType					_type;
	NetAddress					_netAddress = {};
	IocpCoreRef					_iocpCore;

	Set<SessionRef>				_sessions;
	int32						_sessionCount = 0;
	int32						_maxSessionCount = 0;
	SessionFactory				_sessionFactory;

	bool						_isClose = false;
};



///////////////////////////////////////////////////////
//	<	ClientService	>
///////////////////////////////////////////////////////
class ClientService : public Service
{
public:
	ClientService(NetAddress targetAddress, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount = 1);
	virtual ~ClientService() {}

public:
	virtual bool Start() override;
};



///////////////////////////////////////////////////////
//	<	ServerService	>
///////////////////////////////////////////////////////
class ServerService : public Service
{
public:
	ServerService(NetAddress address, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount = 1);
	virtual ~ServerService() {}

public:
	virtual bool		Start() override;
	virtual void		CloseService() override;

private:
	ListenerRef			_listener = nullptr;
};