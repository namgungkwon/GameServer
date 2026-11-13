#include "pch.h"
#include "ClientPacketHandler.h"

#include "DBManager.h"
#include "RedisManager.h"
#include "SessionManager.h"
#include "GameSession.h"


///////////////////////////////////////////////////////
//	<	ClientPacketHandler	>
///////////////////////////////////////////////////////
bool Handle_CL_CREATE_ACCOUNT(PacketSessionRef& session, Protocol::CL_CREATE_ACCOUNT& pkt)
{
    const string id = pkt.id();
    const string password = pkt.password();

    if (id.size() == 0 || password.size() == 0)
    {
        Protocol::LC_CREATE_ACCOUNT res;
        res.set_result(Protocol::CREATEACCOUNT_LENGTH);
        SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(res);
        session->Send(sendBuffer);
        return true;
    }

    GDBManager->DoAsync(Job([id, password, session]() 
        {
            Protocol::LC_CREATE_ACCOUNT res;
            res.set_result(GDBManager->CreateAccount(id, password));
            SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(res);
            session->Send(sendBuffer);
        }));

    return true;
}

bool Handle_CL_LOGIN(PacketSessionRef& session, Protocol::CL_LOGIN& pkt)
{
    PROFILE_BEGIN(L"Handle_CL_LOGIN");
    GDBManager->DoAsync(Job([id = pkt.id(), password = pkt.password(), session]()
        {
            uint64 accountId;
            Protocol::Result result = GDBManager->LoginGame(id, password, accountId);

            if (result != Protocol::SUCCESS)
            {
                Protocol::LC_LOGIN res;
                res.set_result(result);
                SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(res);
                session->Send(sendBuffer);
                return;
            }

            GRedisManager->DoAsync([accountId, session]()
                {
                    std::string token = GRedisManager->SetToken(accountId);

                    Protocol::LC_LOGIN res;
                    res.set_result(Protocol::SUCCESS);
                    res.set_token(token);
                    SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(res);
                    session->Send(sendBuffer);
                });

        }));
    PROFILE_END(L"Handle_CL_LOGIN");
    return true;
}