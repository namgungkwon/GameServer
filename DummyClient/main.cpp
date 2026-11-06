#include "pch.h"

#include "ThreadManager.h"
#include "LoginServerPacketHandler.h"
#include "GameServerPacketHandler.h"

#include "protocol.pb.h"

///////////////////////////////////////////////////////
//	<	ServerSession	>
///////////////////////////////////////////////////////
class ServerSession : public PacketSession
{
public:
	ServerSession(Protocol::ServerType serverType)
	{
		_serverInfo.set_servertype(serverType);
	}

protected:
	virtual void OnConnected() override
	{
		Protocol::ServerType serverType = GetServerType();
		switch (serverType)
		{
		case Protocol::SERVER_TYPE_GAME:
			GConsoleLogger->WriteStdOut(Color::YELLOW, L"GameServer Connected! ");
			break;
		case Protocol::SERVER_TYPE_LOGIN:
			GConsoleLogger->WriteStdOut(Color::YELLOW, L"LoginServer Connected! ");
			break;
		default:
			break;
		}
	}

	//virtual void OnDisconnected() override;
	virtual void OnRecvPacket(BYTE* buffer, int32 len) override
	{
		PacketSessionRef session = GetPacketSessionRef();
		Protocol::ServerType serverType = GetServerType();
		switch (serverType)
		{
		case Protocol::SERVER_TYPE_GAME:
			GameServerPacketHandler::HandlePacket(session, buffer, len);
			break;
		case Protocol::SERVER_TYPE_LOGIN:
			LoginServerPacketHandler::HandlePacket(session, buffer, len);
			break;
		default:
			break;
		}
	}
	//virtual void OnSend(int32 len) override;

public:
	Protocol::ServerInfo GetServerInfo() { return _serverInfo; }
	Protocol::ServerType GetServerType() { return _serverInfo.servertype(); }
	void SetServerInfo(Protocol::ServerInfo serverInfo);

private:
	Protocol::ServerInfo _serverInfo;
};

///////////////////////////////////////////////////////
//	<	CenterServerSession	>
///////////////////////////////////////////////////////
class GameServerSession : public ServerSession
{
public:
	GameServerSession() : ServerSession(Protocol::SERVER_TYPE_GAME) {};
};

///////////////////////////////////////////////////////
//	<	LoginServerSession	>
///////////////////////////////////////////////////////
class LoginServerSession : public ServerSession
{
public:
	LoginServerSession() : ServerSession(Protocol::SERVER_TYPE_LOGIN) {};

protected:
	virtual void OnConnected() override
	{
		Protocol::ServerType serverType = GetServerType();
		switch (serverType)
		{
		case Protocol::SERVER_TYPE_GAME:
			GConsoleLogger->WriteStdOut(Color::YELLOW, L"GameServer Connected! \n");
			break;
		case Protocol::SERVER_TYPE_LOGIN:
			GConsoleLogger->WriteStdOut(Color::YELLOW, L"LoginServer Connected! \n");
			break;
		default:
			break;
		}

		this_thread::sleep_for(1s);
		Protocol::CL_REQ_LOGIN pkt;
		const string id = "sideunging";
		const string password = "namgungkwon";
		pkt.set_id(id);
		pkt.set_password(password);

		SendBufferRef sendBuffer = LoginServerPacketHandler::MakeSendBuffer(pkt);
		Send(sendBuffer);
		GConsoleLogger->WriteStdOut(Color::RED, L"ML_RES_LOGIN (CLIENT -> LOGIN) - id : %s, password : %s\n", ConsoleLog::stringToString(id).c_str(), ConsoleLog::stringToString(password).c_str());
	}
};

int main()
{
	Init_PacketHandler();
	GameServerPacketHandler::Init();
	LoginServerPacketHandler::Init();

	GConsoleLogger->WriteStdOut(Color::WHITE, L"DummyClient\n");
	this_thread::sleep_for(2s);

	ClientServiceRef loginServerService = MakeShared<ClientService>(NetAddress(L"127.0.0.1", Protocol::SERVER_PORT_LOGIN_SERVER_PORT_FOR_CLIENT), MakeShared<IocpCore>(), MakeShared<LoginServerSession>, 1);
	ClientServiceRef gameServerService = MakeShared<ClientService>(NetAddress(L"127.0.0.1", Protocol::SERVER_PORT_GAME_SERVER_PORT_FOR_CLIENT), MakeShared<IocpCore>(), MakeShared<GameServerSession>, 1);

	ASSERT_CRASH(gameServerService->Start());
	ASSERT_CRASH(loginServerService->Start());


	for (size_t i = 0; i < 5; ++i)
	{
		GThreadManager->Launch([=] {
			while (true)
			{
				loginServerService->Process();
			}
			});
	}

	GThreadManager->Join();

	return 0;
}