#pragma once

#include "ConfigLoader.h"
#include "DBConnection.h"
#include "DBConnectionPool.h"
#include "Job.h"

#include "Protocol.pb.h"

struct DBConn
{
    DBConn() { conn = GDBConnectionPool->Pop();  }
    ~DBConn() { GDBConnectionPool->Push(conn); }
    DBConnection* conn;
};

struct DBTransactionGuard
{
    DBConnection* conn = nullptr;
    bool          committed = false;

    explicit DBTransactionGuard(DBConnection* c) : conn(c) { if (conn) conn->SetAutoCommit(false); }
    ~DBTransactionGuard() 
    {
        if (!conn) return;
        if (!committed) conn->Rollback();
        conn->SetAutoCommit(true);   // restore before the connection returns to the pool
    }

    bool Commit() { committed = (conn && conn->Commit()); return committed; }
};

class DBManager
{
public:
	bool Init(const DBConfig& config);
	void DoAsync(Job&& job);

public:
    Protocol::Result CreateAccount(const string& id, const string& password);
    Protocol::Result LoginGame(const string& id, const string& password, OUT uint64& accountId);

private:
	Vector<JobQueueRef> _workers;
	Atomic<uint32> _choiceWorker;
};
