#pragma once
#include "IocpCore.h"

#include "NetAddress.h"

class AcceptEvent;
class ServerService;


///////////////////////////////////////////////////////
//	<	Listener	>
///////////////////////////////////////////////////////
class Listener : public IocpObject
{
public:
	Listener()		= default;
	~Listener();

public:
	bool					StartAccept(ServerServiceRef netAddress);
	void					CloseSocket();

private:
	virtual HANDLE			GetHandle() override { return reinterpret_cast<HANDLE>(_socket); };
	virtual void			Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;
	virtual void			HandleError(const IocpEvent* iocpEvent, const int32 errorCode) override;

private:
	void					RegisterAccept(AcceptEvent* acceptEvent);
	void					ProcessAccept(AcceptEvent* acceptEvent);

protected:
	SOCKET					_socket = INVALID_SOCKET;
	Vector<AcceptEvent*>	_acceptEvents;
	ServerServiceRef		_service;
};

