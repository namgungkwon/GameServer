#include "pch.h"
#include "LoginServerPacketHandler.h"
#include "GameServerPacketHandler.h"

#include "GameServerSession.h"
#include "LoginServerSession.h"
#include "DummyClient.h"

bool Handle_LC_CREATE_ACCOUNT(PacketSessionRef& session, Protocol::LC_CREATE_ACCOUNT& pkt)
{
	LoginServerSessionRef loginSession = static_pointer_cast<LoginServerSession>(session);
	if (loginSession == nullptr)
		return true;

	DummyClientRef dummy = loginSession->GetOwner();
	if (dummy == nullptr)
		return true;

	dummy->Login();

	return true;
}

bool Handle_LC_LOGIN(PacketSessionRef& session, Protocol::LC_LOGIN& pkt)
{
	LoginServerSessionRef loginSession = static_pointer_cast<LoginServerSession>(session);
	if (loginSession == nullptr)
		return true;

	DummyClientRef dummy = loginSession->GetOwner();
	if (dummy == nullptr)
		return true;

	Protocol::Result result = pkt.result();

	if (result == Protocol::LOGIN_INVALID_ACCOUNT)
	{
		dummy->CreateAccount();
		return true;
	}

	dummy->DisconnectLoginServer();

	if (result == Protocol::SUCCESS)
	{
		dummy->RequestEnterGameServer(pkt.token());
	}

	return true;
}
