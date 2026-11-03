#include "pch.h"
#include "DBManager.h"
#include "Jobqueue.h"
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
	uint32 idx = _choiceWorker.fetch_add(1, std::memory_order_relaxed)	% static_cast<uint32>(_workers.size());
	_workers[idx]->DoAsync(std::move(job));
}

bool DBManager::CreateAccount(string id, string password)
{
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
			return false;
	}

	if (guard.Commit() == false) return false;

	return true;
}