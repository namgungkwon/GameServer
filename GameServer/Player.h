#pragma once
#include "Entity.h"
#include "protocol.pb.h"
#include "ClientPacketHandler.h"
#include "GameSession.h"

class Player : public Entity
{
public:
	bool Init(PlayerSummaryData& summary, PlayerLoadData& load);
	void Respawn(const Vector3& respawnLocation) override;
	void GainExp(int32 exp);
	void LevelUp();

public:
	void Send(SendBufferRef& sendBuffer);

public:
	bool InitPlayerLevelData(int32 level);

public:
	PlayerLoadData MakeLoadData();

private:
	USE_LOCK;
	GETSET_WEAK(GameSession, _ownerSession, Session);
	GETSET(uint64, _playerDBId, DBId);
	GET(int32, _requiredExp, RequiredExp);
};

