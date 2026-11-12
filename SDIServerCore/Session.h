#pragma once

#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"
#include "RecvBuffer.h"

class Service;


///////////////////////////////////////////////////////
//	<	Session	>
///////////////////////////////////////////////////////
class Session : public IocpObject
{
	friend class Listener;
	friend class Service;

public:
	Session();
	virtual ~Session();

public:
	/* 외부에서 사용 */
	void					Send(SendBufferRef sendBuffer);
	bool					Connect();
	void					Disconnect();

public:
	/* get & set */
	shared_ptr<Service>		GetService()							{ return _service.lock(); }
	void					SetService(shared_ptr<Service> service) { _service = service; }
	void					SetNetAddress(NetAddress address)		{ _netAddress = address; }
	NetAddress				GetAddress()							{ return _netAddress; }
	SOCKET					GetSocket()								{ return _socket; }
	bool					IsConnected()							{ return _connected; }
	SessionRef				GetSessionRef()							{ return static_pointer_cast<Session>(shared_from_this()); }
	uint64					GetLastReceivedTime()					{ return _lastReceivedTime; }

private:
	/* interface */
	virtual HANDLE			GetHandle() override					{ return reinterpret_cast<HANDLE>(_socket); };
	virtual void			Dispatch(IocpEvent* iocpEvent, int32 numOfBytes = 0) override;
	virtual void			HandleError(IocpEvent* iocpEvent, int32 errorCode) override;

private:
	/* transport */
	bool					RegisterConnect();
	bool					RegisterDisconnect();
	void					RegisterRecv();
	void					RegisterSend();

	void					ProcessConnect();
	void					ProcessDisconnect();
	void					ProcessRecv(int32 numOfBytes);
	void					ProcessSend(int32 numOfBytes);

protected:
	/* overriding for contents code */
	virtual void			OnConnected() {};
	virtual int32			OnRecv(BYTE* buffer, int32 len) { return len; }
	virtual void			OnSend(int32 len) {}
	virtual void			OnDisconnected() {}

private:
	weak_ptr<Service>		_service;
	SOCKET					_socket = INVALID_SOCKET;
	NetAddress				_netAddress = {};
	Atomic<bool>			_connected = false;

private:
	USE_LOCK;

	/* 수신관련 */
	RecvBuffer				_recvBuffer;
	/* 송신 관련 */
	Queue<SendBufferRef>	_sendQueue;
	Atomic<bool>			_sendRegistered = false;

private:
	/* iocpEvent 재사용 */
	ConnectEvent			_connectEvent;
	DisconnectEvent			_disconnectEvent;
	RecvEvent				_recvEvent;
	SendEvent				_sendEvent;

private:
	uint64					_lastReceivedTime;
};



///////////////////////////////////////////////////////
//	<	PacketHeader	>
///////////////////////////////////////////////////////
struct PacketHeader
{
	uint16 size;
	uint16 id;
};



///////////////////////////////////////////////////////
//	<	PacketSession	>
///////////////////////////////////////////////////////
class PacketSession : public Session
{
public:
	PacketSession()				= default;
	virtual ~PacketSession()	= default;

public:
	PacketSessionRef			GetPacketSessionRef() { return static_pointer_cast<PacketSession>(shared_from_this()); }

protected:
	virtual void				OnConnected();
	virtual int32				OnRecv(BYTE* buffer, int32 len) sealed;
	virtual void				OnRecvPacket(BYTE* buffer, int32 len) abstract;
	virtual void				OnDisconnected();
};