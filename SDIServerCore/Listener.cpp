#include "pch.h"
#include "Listener.h"
#include "SocketUtils.h"
#include "IocpEvent.h"
#include "Session.h"


///////////////////////////////////////////////////////
//	<	Listener	>
///////////////////////////////////////////////////////
Listener::~Listener()
{
	CloseSocket();

	for (AcceptEvent* acceptEvent : _acceptEvents)
	{
		cdelete(acceptEvent);
	}
}

bool Listener::StartAccept(ServerServiceRef service)
{
	_service = service;
	if (_service == nullptr)
		return false;

	ASSERT_CRASH(_socket == INVALID_SOCKET);

	_socket = SocketUtils::CreateSocket();

	if (_socket == INVALID_SOCKET)
		return false;

	if (_service->GetIocpCore()->Register(shared_from_this()) == false)
		return false;

	if (SocketUtils::SetReuseAddress(_socket, true) == false)
		return false;

	if (SocketUtils::SetLinger(_socket, 0, 0) == false)
		return false;

	if (SocketUtils::Bind(_socket, _service->GetNetAddress()) == false)
		return false;

	if (SocketUtils::Listen(_socket) == false)
		return false;

	const size_t acceptCount = _service->GetAcceptPoolCount();
	for (int32 i = 0; i < acceptCount; ++i)
	{
		AcceptEvent* acceptEvent = cnew<AcceptEvent>();
		acceptEvent->owner = shared_from_this();
		_acceptEvents.push_back(acceptEvent);
		RegisterAccept(acceptEvent);
	}

	return true;
}

void Listener::CloseSocket()
{
	SocketUtils::Close(_socket);
}

void Listener::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	ASSERT_CRASH(iocpEvent->eventType == EventType::Accept);
	AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(iocpEvent);
	ProcessAccept(acceptEvent);
}

void Listener::HandleError(IocpEvent* iocpEvent, int32 errorCode)
{
	CRASH("Listener IOCP Error");
}

void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
	SessionRef session = _service->CreateSession();

	acceptEvent->Init();
	acceptEvent->_session = session;

	DWORD bytesReceive = 0;

	if (false == SocketUtils::AcceptEx(_socket, session->GetSocket(), session->_recvBuffer.WritePos(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, OUT & bytesReceive, static_cast<LPOVERLAPPED>(acceptEvent)))
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			RegisterAccept(acceptEvent);
		}
	}
}

void Listener::ProcessAccept(AcceptEvent* acceptEvent)
{
	do
	{
		SessionRef session = acceptEvent->_session;

		if (false == SocketUtils::SetUpdateAcceptSocket(session->GetSocket(), _socket))
		{
			break;
		}

		SOCKADDR_IN sockAddress;
		int32 sizeOfSockAddr = sizeof(sockAddress);
		if (SOCKET_ERROR == ::getpeername(session->GetSocket(), OUT reinterpret_cast<PSOCKADDR>(&sockAddress), &sizeOfSockAddr))
		{
			break;
		}

		session->SetNetAddress(NetAddress(sockAddress));
		session->ProcessConnect();

	} while (false);

	RegisterAccept(acceptEvent);
}
