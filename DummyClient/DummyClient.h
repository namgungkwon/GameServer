#pragma once

#include "Protocol.pb.h"

enum
{
	MOVE_TIME = 200,
	MOVE_DIRECTION_CHANGE_MIN = 1000,
	MOVE_DIRECTION_CHANGE_MAX = 5000,

	ATTACK_TIME_MIN = 3000,
	ATTACK_TIME_MAX = 50000,

	CHAT_TIME_MIN = 10000,
	CHAT_TIME_MAX = 20000,

	CHAT_WORLD_TIME = 1000,		// N 미리초마다
	CHAT_WORLD_PERCENT = 5000,  // (1 / N) 의 확률로 보냄
};

class DummyClient : public enable_shared_from_this<DummyClient>
{
public:
	void Init(ClientServiceRef game, ClientServiceRef login, string id, string password);
	void SetLoginServer(ClientServiceRef service) { _loginServerService = service; }
	void SetGameServer(ClientServiceRef service) { _gameServerService = service; }
	void Clear();

	LoginServerSessionRef GetLoginSession() { return _loginServerSession; }
	GameServerSessionRef GetGameSession() { return _gameServerSession; }

public:
	bool ConnectLoginServer();
	bool ConnectGameServer();
	void DisconnectLoginServer();
	void DisconnectGameServer();

public:
	void Process();

public:
	// loginServer
	void Login();
	void CreateAccount();

	// gameServer
	void RequestEnterGameServer(string token);
	void RequestCreateCharacter();
	void RequestEnterGame(uint64 playerId);
	void EnterGame();

	void RequestMove();
	void RequestAttack();
	void RequestChat();
	void RequestWorldChat();

public:
	Protocol::Vector3 GetRndVelocity();
	float GetYaw(float x, float y, float currentYaw);

private:
	ClientServiceRef _loginServerService;
	ClientServiceRef _gameServerService;

private:
	LoginServerSessionRef _loginServerSession;
	GameServerSessionRef _gameServerSession;

private:
	bool _isReady = false;
	bool _isMove = false;
	uint64 nextMoveTick = 0;
	uint64 nextAttackTick = 0;
	uint64 nextChatTick = 0;
	uint64 nextWorldChatTick = 0;
	uint64 nextDirChange = 0;
	uint64 currentTick = 0;
	uint64 deltaTick = 0;

public:
	// login
	string id;
	string password;
	
	string token;

	// game
	uint64 playerId;

	string characterId;
	string characterPassword;
	int32 templateId;

	Protocol::ObjectPosData posData;
	Protocol::ObjectStatData statData;
	Protocol::ObjectSummaryData summaryData;
	Protocol::Vector3 dir;

	string localChat = "localChat";
	string worldChat = "WorldChat";
};