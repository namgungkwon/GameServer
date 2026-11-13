#include "pch.h"
#include "ClientPacketHandler.h"

#include "GameSession.h"
#include "DBManager.h"
#include "RedisManager.h"

#include "World.h"
#include "player.h"
#include "GameScene.h"
#include "MovementComponent.h"


///////////////////////////////////////////////////////
//	<	ClientPacketHandler	>
///////////////////////////////////////////////////////
bool Handle_CG_AUTH_TOKEN(PacketSessionRef& session, Protocol::CG_AUTH_TOKEN& pkt)
{
    GameSessionRef gameSession = static_pointer_cast<GameSession>(session);

    GRedisManager->DoAsync([token = pkt.token(), gameSession = std::move(gameSession)]() {
            
        uint64 accountId;
        Protocol::Result result = GRedisManager->AuthToken(token, OUT accountId);

        if (result != Protocol::SUCCESS)
        {
            Protocol::GC_AUTH_TOKEN res;
            res.set_result(result);

            SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(res);
            gameSession->Send(sendBuffer);
            return;
        }

        GDBManager->DoAsync(Job([accountId, gameSession = std::move(gameSession)]()
            {
                Protocol::GC_AUTH_TOKEN res;
                Vector<Protocol::ObjectSummaryData> summaries;

                res.set_result(GDBManager->GetPlayerList(accountId, OUT summaries));
                
                for (auto& summary : summaries)
                {
                    Protocol::ObjectSummaryData* target = res.add_summaries();
                    *target = summary;
                }

                gameSession->SetAccountDBId(accountId);
                SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(res);
                gameSession->Send(sendBuffer);
            }));

        });
    return true;
}

bool Handle_CG_CREATE_CHARACTER(PacketSessionRef& session, Protocol::CG_CREATE_CHARACTER& pkt)
{
    GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
    if (gameSession->IsAuthenticated() == false) return false;

    const uint64 accountId = gameSession->GetAccountDBId();
    const string name = pkt.name();
    const int32 templateId = pkt.templateid();


    if (name.size() == 0)
    {
        Protocol::GC_CREATE_CHARACTER res;
        res.set_result(Protocol::CREATEACCOUNT_LENGTH);
        SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(res);
        gameSession->Send(sendBuffer);
        return true;
    }

    GDBManager->DoAsync(Job([gameSession = std::move(gameSession), accountId, name = std::move(name), templateId]() {

            Protocol::Result result = GDBManager->CreateCharacter(accountId, name, templateId);

            if (result != Protocol::SUCCESS)
            {
                Protocol::GC_CREATE_CHARACTER res;
                res.set_result(result);
                SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(res);
                gameSession->Send(sendBuffer);
                return;
            }

            GDBManager->DoAsync(Job([accountId, gameSession = std::move(gameSession)]()
                {
                    Protocol::GC_CREATE_CHARACTER res;
                    Vector<Protocol::ObjectSummaryData> summaries;

                    res.set_result(GDBManager->GetPlayerList(accountId, OUT summaries));

                    for (auto& summary : summaries)
                    {
                        Protocol::ObjectSummaryData* target = res.add_summaries();
                        *target = summary;
                    }

                    SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(res);
                    gameSession->Send(sendBuffer);
                }));
        }));

    return true;
}

bool Handle_CG_ENTER_GAME(PacketSessionRef& session, Protocol::CG_ENTER_GAME& pkt)
{
    GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
    if (gameSession->IsAuthenticated() == false) return false;

    // 중복 요청 검사
    if (gameSession->TryEnterGame() == false)
    {
        LOG_WARN(L"Request Client EnterGame Duplicate");
        PROFILE_END(L"Handle_CG_ENTER_GAME");
        return true;
    }

    const uint64 accountId = gameSession->GetAccountDBId();

    //temp
    //uint64 playerId = tempId.fetch_add(1);
    uint64 playerId = pkt.playerid();
    // DB에서 선택한 계정의 목록을 불러옴
    GDBManager->DoAsync(Job([gameSession = std::move(gameSession), accountId, playerId]()
        {
            PlayerSummaryData summary;
            PlayerLoadData load;

            if (GDBManager->GetPlayerDetail(accountId, playerId, OUT &summary, OUT &load) == false)
            {
                gameSession->ReleaseEnterGame();
                Protocol::GC_ENTER_GAME res;
                res.set_success(false);
                SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(res);
                gameSession->Send(sendBuffer);
                return;
            }

            gameSession->SetPlayerDBId(playerId);

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

bool Handle_CG_REVIVE(PacketSessionRef& session, Protocol::CG_REVIVE& pkt)
{
    GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
    if (gameSession->IsAuthenticated() == false) return false;

    PlayerRef player = gameSession->GetPlayer();
    if (player == nullptr)
        return false;

    GameSceneRef scene = player->GetScene();
    if (scene == nullptr)
        return false;

    scene->DoAsync(&GameScene::Request, static_pointer_cast<SceneRequest>(MakeShared<ReviveRequest>(pkt, player)));

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
    job.location = UtilMath::ToServerLocation(pkt.movedata().location());
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


bool Handle_CG_CHAT(PacketSessionRef& session, Protocol::CG_CHAT& pkt)
{
    GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
    if (gameSession->IsAuthenticated() == false) return false;

    PlayerRef player = gameSession->GetPlayer();
    if (player == nullptr)
        return false;

    GameSceneRef scene = player->GetScene();
    if (scene == nullptr)
        return false;

    scene->DoAsync(&GameScene::Request, static_pointer_cast<SceneRequest>(MakeShared<ChatRequest>(pkt, player)));

    return true;
}