#pragma once
#include "Entity.h"
#include "protocol.pb.h"
#include "ClientPacketHandler.h"
#include "GameSession.h"

class Player : public Entity
{
public:
	bool Init(PlayerSummaryData& summary, PlayerLoadData& load);

	PlayerLoadData MakeLoadData();

	void Send(SendBufferRef& sendBuffer);

	GETSET_WEAK(GameSession, _ownerSession, Session);
	GETSET(uint64, _playerDBId, DBId);
	GETSET(int32, _requiredExp, RequiredExp);
};

