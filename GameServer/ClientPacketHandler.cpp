#include "pch.h"
#include "ClientPacketHandler.h"

#include "GameSession.h"
#include "DBManager.h"
#include "job.h"
#include "World.h"
#include "player.h"
#include "GameScene.h"
#include "MovementComponent.h"

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

    //temp
    static Atomic<uint64> tempId = 1;
    uint64 playerId = tempId.fetch_add(1);
    //uint64 playerId = pkt.playerid();
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
            GWorld->DoAsync(&World::EnterPlayer, gameSession, summary, load);
        }));

    return true;
}

bool Handle_CG_LEAVE_GAME(PacketSessionRef& session, Protocol::CG_LEAVE_GAME& pkt)
{
    GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
    if (gameSession->IsAuthenticated() == false) return false;

    gameSession->LeavePlayer();

    return true;
}

bool Handle_CG_MOVE(PacketSessionRef& session, Protocol::CG_MOVE& pkt)
{
    GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
    if (gameSession->IsAuthenticated() == false) return false;

    PlayerRef player = gameSession->GetPlayer();
    if (player == nullptr)
        return false;

    GameSceneRef scene = player->GetScene();
    if (scene == nullptr)
        return false;

    MoveJob job;
    Protocol::Vector3 rawVelocity = pkt.movedata().velocity();
    job.velocity = Vector3(rawVelocity.x(), rawVelocity.y(), rawVelocity.z());
    Protocol::Vector3 rawLocation = pkt.movedata().location();
    job.location = Vector3(rawLocation.x(), rawLocation.y(), rawLocation.z());
    job.yaw = pkt.movedata().yaw();

    player->GetComponent<MovementComponent>()->SetPendingJob(job);

    return true;
}

bool Handle_CG_ATTACK(PacketSessionRef& session, Protocol::CG_ATTACK& pkt)
{
    GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
    if (gameSession->IsAuthenticated() == false) return false;

    PlayerRef player = gameSession->GetPlayer();
    if (player == nullptr)
        return false;
    
    GameSceneRef scene = player->GetScene();
    if (scene == nullptr)
        return false;

    scene->DoAsync(&GameScene::Request, static_pointer_cast<SceneRequest>(MakeShared<AttackRequest>(pkt, player)));

    return true;
}
