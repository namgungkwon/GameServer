#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"


///////////////////////////////////////////////////////
//	<	Session	>
///////////////////////////////////////////////////////
Session::Session()
{
	_socket = SocketUtils::CreateSocket();
	_lastReceivedTime = ::GetTickCount64();
}

Session::~Session()
{
	SocketUtils::Close(_socket);
}

void Session::Send(SendBufferRef sendBuffer)
{
	if (IsConnected() == false)
		return;

	bool registerSend = false;

	{
		WRITE_LOCK;
		_sendQueue.push(std::move(sendBuffer));

		if (_sendRegistered.exchange(true) == false)
			registerSend = true;
	}

	if (registerSend)
		RegisterSend();
}

bool Session::Connect()
{
	return RegisterConnect();
}

void Session::Disconnect()
{
	if (_connected.exchange(false) == false)
		return;

	RegisterDisconnect();
}

void Session::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	switch (iocpEvent->eventType)
	{
	case EventType::Connect:
		ProcessConnect();
		break;
	case EventType::Disconnect:
		ProcessDisconnect();
		break;
	case EventType::Recv:
		ProcessRecv(numOfBytes);
		break;
	case EventType::Send:
		ProcessSend(numOfBytes);
		break;
	default:
		break;
	}
}

void Session::HandleError(IocpEvent* iocpEvent, int32 errorCode)
{
	switch (iocpEvent->eventType)
	{
	case EventType::Connect:
		_connectEvent.owner = nullptr;
		break;
	case EventType::Disconnect:
		LOG_ERROR(L"Iocp Disconnect Failed");
		_disconnectEvent.owner = nullptr;
		break;
	case EventType::Recv:
		_recvEvent.owner = nullptr;
		break;
	case EventType::Send:
		_sendEvent.owner = nullptr;
		break;
	default:
		break;
	}

	Disconnect();
}

bool Session::RegisterConnect()
{
	if (IsConnected())
		return false;

	if (GetService()->GetServiceType() != ServiceType::Client)
		return false;

	if (SocketUtils::SetReuseAddress(_socket, true) == false)
		return false;

	if (SocketUtils::BindAnyAddress(_socket, 0) == false)
		return false;

	_connectEvent.Init();
	_connectEvent.owner = shared_from_this();

	DWORD numOfBytes = 0;
	SOCKADDR_IN sockAddr = GetService()->GetNetAddress().GetSockAddr();
	if (false == SocketUtils::ConnectEx(_socket, reinterpret_cast<PSOCKADDR>(&sockAddr), sizeof(sockAddr), nullptr, 0, &numOfBytes, static_cast<LPOVERLAPPED>(&_connectEvent)))
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			_connectEvent.owner = nullptr;
			return false;
		}
	}

	return true;
}

bool Session::RegisterDisconnect()
{
	_disconnectEvent.Init();
	_disconnectEvent.owner = shared_from_this();

	if (false == SocketUtils::DisconnextEx(_socket, static_cast<LPOVERLAPPED>(&_disconnectEvent), TF_REUSE_SOCKET, 0))
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			_disconnectEvent.owner = nullptr;
			return false;
		}
	}
	return true;
}

void Session::RegisterRecv()
{
	if (IsConnected() == false)
		return;

	_recvEvent.Init();
	_recvEvent.owner = shared_from_this();

	WSABUF wsaBuf;
	wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer.WritePos());
	wsaBuf.len = static_cast<ULONG>(_recvBuffer.FreeSize());

	DWORD numOfBytes = 0;
	DWORD flags = 0;

	if (SOCKET_ERROR == ::WSARecv(_socket, &wsaBuf, 1, OUT & numOfBytes, OUT & flags, static_cast<LPOVERLAPPED>(&_recvEvent), nullptr))
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			_recvEvent.owner = nullptr;
			Disconnect();
		}
	}
}

void Session::RegisterSend()
{
	if (IsConnected() == false)
		return;

	_sendEvent.Init();
	_sendEvent.owner = shared_from_this();

	{
		WRITE_LOCK;

		int32 writeSize = 0;
		while (_sendQueue.empty() == false)
		{
			SendBufferRef sendBuffer = _sendQueue.front();

			writeSize += sendBuffer->WriteSize();

			_sendQueue.pop();
			_sendEvent.sendBuffers.push_back(std::move(sendBuffer));
		}
	}

	Vector<WSABUF> wsaBufs;
	wsaBufs.reserve(_sendEvent.sendBuffers.size());
	for (SendBufferRef sendBuffer : _sendEvent.sendBuffers)
	{
		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(sendBuffer->Buffer());
		wsaBuf.len = static_cast<ULONG>(sendBuffer->WriteSize());
		wsaBufs.push_back(wsaBuf);
	}

	DWORD numOfBytes = 0;
	if (SOCKET_ERROR == ::WSASend(_socket, wsaBufs.data(), static_cast<DWORD>(wsaBufs.size()), OUT & numOfBytes, 0, &_sendEvent, nullptr))
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			_sendEvent.owner = nullptr;
			_sendEvent.sendBuffers.clear();
			_sendRegistered.store(false);
			Disconnect();
		}
	}
}

void Session::ProcessConnect()
{
	_connectEvent.owner = nullptr;
	_connected.store(true);

	// 세션등록
	if (GetService()->AddSession(GetSessionRef()) == false)
	{
		Disconnect();
		return;
	}

	//컨텐츠 코드 재정의 
	OnConnected();

	// 수신등록
	RegisterRecv();
}

void Session::ProcessDisconnect()
{
	_disconnectEvent.owner = nullptr;

	OnDisconnected();

	GetService()->ReleaseSession(GetSessionRef());
}

void Session::ProcessRecv(int32 numOfBytes)
{
	_recvEvent.owner = nullptr;
	_lastReceivedTime = ::GetTickCount64();

	if (numOfBytes == 0)
	{
		Disconnect();
		return;
	}

	if (GServerStats)
	{
		GServerStats->network.recvBytes.fetch_add(numOfBytes);
	}


	if (_recvBuffer.OnWrite(numOfBytes) == false)
	{
		Disconnect();
		return;
	}

	const int32 dataSize = _recvBuffer.DataSize();
	const int32 processLen = OnRecv(_recvBuffer.ReadPos(), dataSize);
	if (processLen < 0 || dataSize < processLen || _recvBuffer.OnRead(processLen) == false)
	{
		Disconnect();
		return;
	}

	_recvBuffer.Clean();

	RegisterRecv();
}

void Session::ProcessSend(int32 numOfBytes)
{
	_sendEvent.owner = nullptr;
	_sendEvent.sendBuffers.clear();

	if (numOfBytes == 0)
	{
		Disconnect();
		return;
	}

	if (GServerStats)
	{
		GServerStats->network.sendBytes.fetch_add(numOfBytes);
		GServerStats->network.sendPackets.fetch_add(1);
	}

	OnSend(numOfBytes);

	WRITE_LOCK
		if (_sendQueue.empty())
			_sendRegistered.store(false);
		else
			RegisterSend();
}



///////////////////////////////////////////////////////
//	<	PacketSession	>
///////////////////////////////////////////////////////
void PacketSession::OnConnected()
{
	if (GServerStats)
	{
		GServerStats->network.connectedSessions.fetch_add(1);
		GServerStats->network.totalConnections.fetch_add(1);
	}
}

void PacketSession::OnDisconnected()
{
	if (GServerStats)
	{
		GServerStats->network.connectedSessions.fetch_sub(1);
	}
}

int32 PacketSession::OnRecv(BYTE* buffer, int32 len)
{
	int32 processLen = 0;
	while (true)
	{
		const int32 dataSize = len - processLen;

		if (dataSize < sizeof(PacketHeader))
			break;

		const PacketHeader header = *(reinterpret_cast<PacketHeader*>(&buffer[processLen]));

		if (dataSize < header.size)
			break;

		if (header.size > Config::Packet::MAX_RECV_PACKET_SIZE)
			return -1;

		OnRecvPacket(&buffer[processLen], header.size);

		if (GServerStats)
		{
			GServerStats->network.recvPackets.fetch_add(1);
		}

		processLen += header.size;
	}

	return processLen;
}
