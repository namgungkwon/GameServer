#pragma once

#include "Protocol.pb.h"


///////////////////////////////////////////////////////
//	<	SceneRequest	>
///////////////////////////////////////////////////////
struct SceneRequest : public enable_shared_from_this<SceneRequest>
{
public:
	virtual ~SceneRequest() = default;

public:
	virtual void Execute(const GameSceneRef& scene, uint64 currentTick) = 0;
};

///////////////////////////////////////////////////////
//	<	AttackRequest	>
///////////////////////////////////////////////////////
struct AttackRequest : public SceneRequest
{
public:
	AttackRequest(Protocol::CG_ATTACK pkt, EntityRef attacker) : pkt(pkt), attacker(attacker) {};

public:
	virtual void Execute(const GameSceneRef& scene, uint64 currentTick);

private:
	Protocol::CG_ATTACK pkt;
	EntityRef attacker;
};

///////////////////////////////////////////////////////
//	<	ReviveRequest	>
///////////////////////////////////////////////////////
struct ReviveRequest : public SceneRequest
{
public:
	ReviveRequest(Protocol::CG_REVIVE pkt, PlayerRef target) : pkt(pkt), target(target) {};

public:
	virtual void Execute(const GameSceneRef& scene, uint64 currentTick);
	
private:
	Protocol::CG_REVIVE pkt;
	PlayerRef target;
};

///////////////////////////////////////////////////////
//	<	ChatRequest	>
///////////////////////////////////////////////////////
struct ChatRequest : public SceneRequest
{
public:
	ChatRequest(Protocol::CG_CHAT pkt, PlayerRef player) : pkt(pkt), player(player) {};

public:
	virtual void Execute(const GameSceneRef& scene, uint64 currentTick);
private:
	Protocol::CG_CHAT pkt;
	PlayerRef player;
};