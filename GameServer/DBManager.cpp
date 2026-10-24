#include "pch.h"
#include "DBManager.h"
#include "Jobqueue.h"
#include "DBConnectionPool.h"
#include "DBBind.h"


bool DBManager::Init(const DBConfig& config)
{
    wstring connStr;

    if (config.trustedConnection)
    {
        connStr = 
            L"Driver={ODBC Driver 18 for SQL Server};		\
		    Server=(localdb)\\ProjectModels;				\
		    Database=portfolio;								\
		    Trusted_Connection=Yes;";
    }
    else
    {
        // TODO
    }

    if (GDBConnectionPool->Connect(config.threads, connStr.c_str()) == false)
        LOG_ERROR(L"Failed to DB Connect");

    _workers.resize(config.threads);
    for (JobQueueRef& worker : _workers)
    {
        worker = MakeShared<JobQueue>();
    }
 
    return true;
}

void DBManager::DoAsync(Job&& job)
{
    uint32 idx = _choiceWorker.fetch_add(1, std::memory_order_relaxed)
        % static_cast<uint32>(_workers.size());
    _workers[idx]->DoAsync(std::move(job));
}

bool DBManager::GetPlayerDetail(uint64 accountDBId, int64 playerId, PlayerSummaryData* summary, PlayerLoadData* load)
{
    DBConnection* dbConn = GDBConnectionPool->Pop();
    
    int32 oTemplateId;
    WCHAR oName[100] = {};
    int32 oLevel;
    float oPosX, oPosY, oPosZ;
    float oYaw;
    int32 oHp, oMp;
    uint64 oExp;
    
    DBBind<2, 10> bind(*dbConn,
        L"SELECT    p.templateId, p.name, p.level, "
        L"          pd.pos_x, pd.pos_y, pd.pos_z, pd.yaw, "
        L"          pd.hp, pd.mp, pd.exp "
        L"FROM Players as p "
        L"JOIN PlayerDetail as pd "
        L"  ON p.playerid = pd.playerid "
        L"WHERE p.accountId = ? AND p.playerId = ? "
    );

    bind.BindParam(0, accountDBId);
    bind.BindParam(1, playerId);
    bind.BindCol(0, oTemplateId);
    bind.BindCol(1, oName, 100);
    bind.BindCol(2, oLevel);
    bind.BindCol(3, oPosX);
    bind.BindCol(4, oPosY);
    bind.BindCol(5, oPosZ);
    bind.BindCol(6, oYaw);
    bind.BindCol(7, oHp);
    bind.BindCol(8, oMp);
    bind.BindCol(9, oExp);

    if (bind.Execute() == false || dbConn->Fetch() == false)
        return false;

    summary->playerId = playerId;
    summary->templateId = oTemplateId;
    summary->name = wstring(oName);
    summary->level = oLevel;

    load->location = { oPosX, oPosY, oPosZ };
    load->yaw = oYaw;
    load->hp = oHp;
    load->mp = oMp;
    load->exp = oExp;

    return true;
}
