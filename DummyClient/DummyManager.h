#pragma once

#include "jobQueue.h"

enum
{
	DummyTick = 50,
};

class DummyManager : public JobQueue
{
public:
	void Start();
	void Clear();

public:
	void UpdateTick();
public:
	void AddDummyClient(int32 count);

private:
	Vector<DummyClientRef> dummyList;
	ClientServiceRef _loginServerService;
	ClientServiceRef _gameServerService;

private:
	Atomic<bool> _isStart = false;
};

