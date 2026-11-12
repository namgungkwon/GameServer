#include "pch.h"
#include "DBManager.h"
#include "Jobqueue.h"

#include "GameData.h"
#include "DBBind.h"
#include "player.h"


///////////////////////////////////////////////////////
//	<	DBManager	>
///////////////////////////////////////////////////////
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

Protocol::Result DBManager::CreateCharacter(uint64 accountDBId, const string& name, int32 templateId)
{
	DBConn dbConn;
	DBTransactionGuard guard(dbConn.conn);

	WCHAR newId[20];
	wstring wName = Utils::ToWString(name);
	::wcsncpy_s(newId, wName.c_str(), wName.size());

	uint64 oPlayerId;
	{
		DBBind<3, 0> bind(*dbConn.conn,
			L"INSERT INTO Players (accountId, name, templateId) "
			L"VALUES (?, ?, ?) ");

		bind.BindParam(0, accountDBId);
		bind.BindParam(1, newId, static_cast<int32>(wName.size()));
		bind.BindParam(2, templateId);

		if (bind.Execute() == false)
		{
			return Protocol::CREATEACCOUNT_DUPLICATION;
		}
	}

	{
		DBBind<1, 1> bind(*dbConn.conn,
			L"SELECT playerId "
			L"FROM players "
			L"WHERE name = ?"
		);

		bind.BindParam(0, newId, static_cast<int32>(wName.size()));
		bind.BindCol(0, oPlayerId);

		if (bind.Execute() == false || bind.Fetch() == false)
			return Protocol::DB_ERROR;
	}

	{
		float posX	= Config::CreatePlayer::pos_x;
		float posY	= Config::CreatePlayer::pos_y;
		float posZ	= Config::CreatePlayer::pos_z;
		float yaw	= 0;
		float exp	= 0;

		DBBind<8, 0> bind(*dbConn.conn,
			L"INSERT INTO playerDetail (playerId, pos_x, pos_y, pos_z, yaw, hp, mp, exp) "
			L"VALUES (?, ?, ?, ?, ?, ?, ?, ?)"	);

		bind.BindParam(0, oPlayerId);
		bind.BindParam(1, posX);
		bind.BindParam(2, posY);
		bind.BindParam(3, posZ);
		bind.BindParam(4, yaw);
		bind.BindParam(5, GGameData->GetPlayerData(1)->maxHp);
		bind.BindParam(6, GGameData->GetPlayerData(1)->maxMp);
		bind.BindParam(7, exp);

		if (bind.Execute() == false)
		{
			return Protocol::DB_ERROR;
		}
	}

	{
		DBBind<1, 1> bind(*dbConn.conn,
			L"SELECT playerId "
			L"FROM players "
			L"WHERE name = ?"
		);

		bind.BindParam(0, newId, static_cast<int32>(wName.size()));
		bind.BindCol(0, oPlayerId);

		if (bind.Execute() == false || bind.Fetch() == false)
			return Protocol::DB_ERROR;
	}

	if (guard.Commit() == false)
		return Protocol::DB_ERROR;

	return Protocol::SUCCESS;
}

Protocol::Result DBManager::GetPlayerList(uint64 accoutDBId, OUT Vector<Protocol::ObjectSummaryData>& summaries)
{

	uint64 oPlayerId;
	int32 oTemplateId;
	WCHAR oName[255];
	int32 oLevel;

	{
		DBConn dbConn;


		DBBind<1, 4> bind(*dbConn.conn,
			L"SELECT playerId, templateId, name, level "
			L"FROM players "
			L"WHERE accountId = ?;");

		bind.BindParam(0, accoutDBId);
		bind.BindCol(0, oPlayerId);
		bind.BindCol(1, oTemplateId);
		bind.BindCol(2, oName);
		bind.BindCol(3, oLevel);

		if (bind.Execute() == false)
			return Protocol::DB_ERROR;

		while (bind.Fetch())
		{
			Protocol::ObjectSummaryData summary;
			summary.set_objectid(oPlayerId);
			summary.set_name(Utils::ToString(wstring(oName)));
			summary.set_templateid(oTemplateId);
			summary.set_level(oLevel);
			summaries.push_back(std::move(summary));
		}
	}
	
	return Protocol::SUCCESS;
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

	if (bind.Execute() == false || bind.Fetch() == false)
		return false;

	summary->playerId		= playerId;
	summary->templateId		= oTemplateId;
	summary->name			= wstring(oName);
	summary->level			= oLevel;

	load->location			= { oPosX, oPosY, oPosZ };
	load->yaw				= oYaw;
	load->hp				= oHp;
	load->mp				= oMp;
	load->exp				= oExp;

	return true;
}

void DBManager::SavePlayer(uint64 playerId, const PlayerLoadData* load)
{
	DBConn dbConn;

	int64 hp	= load->hp;
	int64 mp	= load->mp;
	int64 exp	= load->exp;
	float x		= load->location.x;
	float y		= load->location.y;
	float z		= load->location.z;
	float yaw	= load->yaw;

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
