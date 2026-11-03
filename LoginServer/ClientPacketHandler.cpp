#include "pch.h"
#include "ClientPacketHandler.h"


#include "SessionManager.h"
#include "GameSession.h"


///////////////////////////////////////////////////////
//	<	ClientPacketHandler	>
///////////////////////////////////////////////////////
bool Handle_CL_CREATE_ACCOUNT(PacketSessionRef& session, Protocol::CL_CREATE_ACCOUNT& pkt)
{
    GameSessionRef clientSession = static_pointer_cast<GameSession>(session);

    const string id = pkt.id();
    const string password = pkt.password();
    return true;
}

bool Handle_CL_LOGIN(PacketSessionRef& session, Protocol::CL_LOGIN& pkt)
{
    return true;
}
/*bool Handle_CL_REQ_LOGIN(PacketSessionRef& session, Protocol::CL_REQ_LOGIN& pkt)
{
    ClientSessionRef clientSession = static_pointer_cast<ClientSession>(session);

    const string id = pkt.id();
    const string password = pkt.password();
    GConsoleLogger->WriteStdOut(Color::RED, L"CL_REQ_LOGIN (CLIENT -> LOGIN) - Id : %s, Password : %s\n", ConsoleLog::stringToString(id).c_str(), ConsoleLog::stringToString(password).c_str());

    {
        Protocol::LM_REQ_LOGIN reqPkt;
        const uint64 clientId = clientSession->GetClientId();
        reqPkt.set_clientid(clientId);
        reqPkt.set_id(id);
        reqPkt.set_password(password);

        SendBufferRef sendBuffer = CenterServerPacketHandler::MakeSendBuffer(reqPkt);

        ServerSessionRef serverSession = GServerSessionManager->GetServerSession();
        serverSession->Send(sendBuffer);
        GConsoleLogger->WriteStdOut(Color::RED, L"LM_REQ_LOGIN (LOGIN -> CENTER) - clientId : %Ild, Id : %s, Password : %s\n", clientId, ConsoleLog::stringToString(id).c_str(), ConsoleLog::stringToString(password).c_str());
    }

    return false;
}

bool Handle_CL_REQ_GAMESERVERLIST(PacketSessionRef& session, Protocol::CL_REQ_GAMESERVERLIST& pkt)
{
    ClientSessionRef clientSession = static_pointer_cast<ClientSession>(session);

    const string token = pkt.token();
    uint64 clientId = clientSession->GetClientInfo().clientid();
    GConsoleLogger->WriteStdOut(Color::RED, L"CL_REQ_GAMESERVERLIST (CLIENT -> LOGIN) - Token = %s\n", ConsoleLog::stringToString(token).c_str());

    {
        Protocol::LM_REQ_GAMESERVERLIST reqPkt;
        reqPkt.set_clientid(clientId);
        reqPkt.set_token(token);

        SendBufferRef sendBuffer = CenterServerPacketHandler::MakeSendBuffer(reqPkt);

        ServerSessionRef serverSession = GServerSessionManager->GetServerSession();
        serverSession->Send(sendBuffer);
        GConsoleLogger->WriteStdOut(Color::RED, L"LM_REQ_GAMESERVERLIST (LOGIN -> CENTER) - clientId = %lld, Token = %s\n", clientId, ConsoleLog::stringToString(token).c_str());
    }

    return false;
}*/
