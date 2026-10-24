#include "pch.h"
#include "ClientPacketHandler.h"

#include "GameSession.h"
#include "DBManager.h"
#include "job.h"
#include "World.h"

bool Handle_CG_ENTER_GAME(PacketSessionRef& session, Protocol::CG_ENTER_GAME& pkt)
{
    GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
    if (gameSession->IsAuthenticated() == false) return false;


    // 중복 요청 검사
    if (gameSession->TryEnterGame() == false)
    {
        LOG_WARN(L"Request Client EnterGame Duplicate");
        return true;
    }

    uint64 accountId = gameSession->GetAccountDBId();
    uint64 playerId = pkt.playerid();
    // DB에서 선택한 계정의 목록을 불러옴
    GDBManager->DoAsync(Job([gameSession, accountId, playerId]()
        {
            PlayerSummaryData summary;
            PlayerLoadData load;

            if (GDBManager->GetPlayerDetail(accountId, playerId, &summary, &load) == false)
            {
                gameSession->ReleaseEnterGame();
                Protocol::GC_ENTER_GAME res;
                res.set_success(false);
                SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(res);
                gameSession->Send(sendBuffer);
                return;
            }

            gameSession->SetPlayerDBId(playerId);

            // Todo : World에 넣기
        }));

    return true;
}

bool Handle_CG_LEAVE_GAME(PacketSessionRef& session, Protocol::CG_LEAVE_GAME& pkt)
{
    GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
    if (gameSession->IsAuthenticated() == false) return false;


    return true;
}
