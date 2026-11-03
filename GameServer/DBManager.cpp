#include "pch.h"
#include "DBManager.h"
#include "Jobqueue.h"

#include "DBBind.h"
#include "player.h"


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
	uint32 idx = _choiceWorker.fetch_add(1, std::memory_order_relaxed)	% static_cast<uint32>(_workers.size());
	_workers[idx]->DoAsync(std::move(job));
}

bool DBManager::GetPlayerDetail(uint64 accountDBId, int64 playerId, PlayerSummaryData* summary, PlayerLoadData* load)
{
	DBConn dbConn;

	int32 oTemplateId;
	WCHAR oName[100] = {};
	int32 oLevel;
	float oPosX, oPosY, oPosZ;
	float oYaw;
	int32 oHp, oMp;
	uint64 oExp;

	DBBind<2, 10> bind(*dbConn.conn,
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

	if (bind.Execute() == false || dbConn.conn->Fetch() == false)
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

void DBManager::SavePlayer(uint64 playerId, const PlayerLoadData* load)
{
	DBConn dbConn;

	int32 hp = load->hp;
	int32 mp = load->mp;
	int32 exp = load->exp;
	float x = load->location.x;
	float y = load->location.y;
	float z = load->location.z;
	float yaw = load->yaw;

	DBBind<8, 0> bind(*dbConn.conn,
		L"UPDATE PlayerDetail "
		L"SET hp=?, mp=?, exp=?, pos_x=?, pos_y=?, pos_z=?, yaw=? "
		L"WHERE playerid=?");

	bind.BindParam(0, hp);
	bind.BindParam(1, mp);
	bind.BindParam(2, exp);
	bind.BindParam(3, x);
	bind.BindParam(4, y);
	bind.BindParam(5, z);
	bind.BindParam(6, yaw);
	bind.BindParam(7, playerId);

	if (bind.Execute() == false)
		return;
}

void DBManager::SaveLevelUp(uint64 playerId, int32 level)
{
	DBConn dbConn;

	DBBind<2, 0> bind(*dbConn.conn,
		L"Update Players "
		L"SET level=? "
		L"WHERE playerid=?");

	bind.BindParam(0, level);
	bind.BindParam(1, playerId);

	if (bind.Execute() == false)
		return;
}
