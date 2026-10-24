#pragma once
#include "Entity.h"
#include "protocol.pb.h"

class Player : public Entity
{
public:
	bool Init(PlayerSummaryData& summary, PlayerLoadData& load);

public:
	void MakePkt(OUT Protocol::GC_ENTER_GAME& pkt);

	GETSET_WEAK(GameSession, _ownerSession, Session);
	GETSET(uint64, _dbId, DBId);
	GETSET(uint64, _templateId, TemplateId);
	GETSET(int32, _exp, Exp);
	GETSET(int32, _requiredExp, RequiredExp);
};

