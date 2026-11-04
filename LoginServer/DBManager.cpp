#include "pch.h"
#include "DBManager.h"
#include "Jobqueue.h"
#include "DBBind.h"

#include "Protocol.pb.h"


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

Protocol::Result DBManager::CreateAccount(const string& id, const string& password)
{
	if (id.size() > 255 || password.size() > 100)
		return Protocol::CREATEACCOUNT_LENGTH;

	DBConn dbConn;
	DBTransactionGuard guard(dbConn.conn);

	// Insert Account
	{
		WCHAR newId[255];
		wstring wId = Utils::ToWString(id);
		::wcsncpy_s(newId, wId.c_str(), 254);

		WCHAR newPassword[100];
		wstring wPassword = Utils::ToWString(password);
		::wcsncpy_s(newPassword, wPassword.c_str(), 99);

		DBBind<2, 0> bind(*dbConn.conn,
			L"INSERT INTO Accounts (id, password) "
			L"VALUES (?, ?);");

		bind.BindParam(0, newId);
		bind.BindParam(1, newPassword);

		if (bind.Execute() == false)
		{
			return Protocol::CREATEACCOUNT_DUPLICATION;
		}
	}

	if (guard.Commit() == false)
	{
		return Protocol::DB_ERROR;
	}

	return Protocol::SUCCESS;
}

Protocol::Result DBManager::LoginGame(const string& id, const string& password, OUT uint64& accountId)
{
	WCHAR newId[255];
	wstring wId = Utils::ToWString(id);
	::wcsncpy_s(newId, wId.c_str(), 254);

	uint64 oAccountId;
	WCHAR oPassword[100] = {};

	// GetAccountInfo
	{
		DBConn dbConn;
		
		DBBind<1, 2> bind(*dbConn.conn,
			L"SELECT password, accountid "
			L"FROM Accounts "
			L"WHERE id = ?;");

		bind.BindParam(0, newId);
		bind.BindCol(0, oPassword);
		bind.BindCol(1, oAccountId);

		if (bind.Execute() == false)
			return Protocol::DB_ERROR;

		if (bind.Fetch() == false)
			return Protocol::LOGIN_INVALID_ACCOUNT;
	}

	if (wstring(oPassword) != Utils::ToWString(password))
	{
		return Protocol::LOGIN_INVALID_ACCOUNT;
	}

	accountId = oAccountId;

	return Protocol::SUCCESS;
}
