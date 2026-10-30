#pragma once
#include "Entity.h"
#include "protocol.pb.h"
#include "ClientPacketHandler.h"
#include "GameSession.h"

class Player : public Entity
{
public:
	bool Init(PlayerSummaryData& summary, PlayerLoadData& load);
	void Respawn() override;

public:
	void Send(SendBufferRef& sendBuffer);

public:
	PlayerLoadData MakeLoadData();

private:
	GETSET_WEAK(GameSession, _ownerSession, Session);
	GETSET(uint64, _playerDBId, DBId);
	GET(int32, _requiredExp, RequiredExp);
};

