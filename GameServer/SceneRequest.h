#pragma once

#include "Protocol.pb.h"

struct SceneRequest : public enable_shared_from_this<SceneRequest>
{
public:
	virtual ~SceneRequest() = default;

public:
	virtual void Execute(GameSceneRef scene, uint64 currentTick) = 0;
};

struct AttackRequest : public SceneRequest
{
public:
	AttackRequest(Protocol::CG_ATTACK pkt, EntityRef attacker) : pkt(pkt), attacker(attacker) {};

public:
	virtual void Execute(GameSceneRef scene, uint64 currentTick);

private:
	Protocol::CG_ATTACK pkt;
	EntityRef attacker;
};