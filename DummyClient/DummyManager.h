#pragma once

#include "jobQueue.h"

enum
{
	DummyTick = 50,
	SimulatorDummyMaxCount = 300
};

useRef_Class(DummySimulator)

class DummySimulator : public JobQueue
{
public:
	void Start();
	void Process();
	void AddDummy(DummyClientRef dummy);

public:
	Vector<DummyClientRef> _dummyList;
	Atomic<bool> _isStart = false;
};

class DummyManager : public JobQueue
{
public:
	void Start();
	void Clear();

public:
	void AddDummyClient(int32 count);

private:
	ClientServiceRef _loginServerService;
	ClientServiceRef _gameServerService;

private:
	int64 _dummyCount = 0;
	Atomic<bool> _isStart = false;
	Vector<DummySimulatorRef> _simulators;
};

