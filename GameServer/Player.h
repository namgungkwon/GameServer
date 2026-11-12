#pragma once
#include "Entity.h"
#include "protocol.pb.h"
#include "ClientPacketHandler.h"
#include "GameSession.h"


///////////////////////////////////////////////////////
//	<	Player	>
///////////////////////////////////////////////////////
class Player : public Entity
{
public:
	~Player();
public:
	bool						Init(const PlayerSummaryData& summary, const PlayerLoadData& load);
	void						Respawn(const Vector3& respawnLocation) override;
	void						GainExp(uint64 exp);
	void						LevelUp();
	bool						ShouldSendMovePacket(uint64 now);

public:
	void						Send(const SendBufferRef& sendBuffer);

public:
	bool						InitPlayerLevelData(int32 level);

public:
	PlayerLoadData				MakeLoadData();

private:
	USE_LOCK;
	GETSET_WEAK(GameSession,	_ownerSession,		Session);
	GETSET		(uint64,		_playerDBId,		DBId);
	GET			(uint64,		_requiredExp,		RequiredExp);

private:
	uint64						_nextSendMoveTick = 0;
};

