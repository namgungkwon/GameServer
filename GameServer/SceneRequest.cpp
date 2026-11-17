#include "pch.h"
#include "SceneRequest.h"

#include "GameData.h"

#include "Skill.h"

#include "GameScene.h"
#include "World.h"
#include "Zone.h"
#include "Player.h"
#include "Monster.h"

#include "CombatSystem.h"

#include "CombatComponent.h"
#include "StatComponent.h"
#include "SkillComponent.h"
#include "MovementComponent.h"

#pragma optimize("", off)
///////////////////////////////////////////////////////
//	<	AttackRequest	>
///////////////////////////////////////////////////////
void AttackRequest::Execute(const GameSceneRef& scene, uint64 currentTick)
{
	Profile p(L"AttackRequest::Execute");

	if (attacker->GetState() == EntityState::Dead)
		return;

	GameSceneRef playerScene = attacker->GetScene();

	if (playerScene != scene)
	{
		playerScene->DoAsync(&GameScene::Request, shared_from_this());
		return;
	}

	Protocol::ObjectPosData posData = pkt.posdata();
	Vector3 attackPos = UtilMath::ToServerLocation(posData.location());

	float posDiffSq = attacker->GetLocation().DistanceSq(attackPos);
	if (posDiffSq > Config::Player::ATTACK_TOLERANCE_LENGTHSQ)
	{
		// 이상패킷
		LOG_WARN(L"[SuspiciousPacket] Distance AttackPos from PlayerPos is to far -> distance :" + std::to_wstring(sqrt(posDiffSq)));

		if (GServerStats)
		{
			GServerStats->game.suspiciousPacket.fetch_add(1);
		}
		return;
	}

	attacker->SetLocation(UtilMath::ToServerLocation(posData.location()));
	attacker->SetYaw(posData.yaw());

	CombatComponent* aCombat = attacker->GetComponent<CombatComponent>();
	SkillComponent* aSkill = attacker->GetComponent<SkillComponent>();
	StatComponent* aStat = attacker->GetComponent<StatComponent>();
	MovementComponent* aMovement = attacker->GetComponent<MovementComponent>();

	if (aCombat == nullptr || aSkill == nullptr || aStat == nullptr)
		return;

	const uint32 skillId = pkt.skillid();

	if (aSkill->HasSkill(skillId) == false)
		return;

	if (aSkill->CooldownReady(skillId, currentTick) == false)
		return;

	SkillInstanceRef skillInstance = aSkill->UseSkill(skillId, currentTick);

	{
		Protocol::GC_ATTACK res;
		attacker->MakePkt(*res.mutable_posdata());
		res.set_skillid(skillId);
		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(res);
		scene->BroadCastAdjacentZone(attacker->GetZone(), sendBuffer, attacker->GetObjectId());
	}

	if (pkt.targets().size() == 0)
		return;

	// 타겟 선별
	Vector<EntityRef> targets;
	for (uint64 targetId : pkt.targets())
	{
		if (EntityRef entity = GWorld->FindEntity(targetId))
			targets.push_back(std::move(entity));
	}

	skillInstance->Execute(targets);
}
#pragma optimize("", on)

///////////////////////////////////////////////////////
//	<	ReviveRequest	>
///////////////////////////////////////////////////////
void ReviveRequest::Execute(const GameSceneRef& scene, uint64 currentTick)
{
	if (target->GetState() != EntityState::Dead)
		return;

	GameSceneRef targetScene = target->GetScene();
	ZoneRef targetZone = target->GetZone();

	if (targetScene == nullptr || targetZone == nullptr)
		return;

	if (targetScene != scene)
	{
		targetScene->DoAsync(&GameScene::Request, shared_from_this());
		return;
	}

	Protocol::RespawnOption option = pkt.option();

	Vector3 currentLocation = target->GetLocation();
	Vector3 respawnLocation;
	
	switch (option)
	{
	case Protocol::RESPAWNOPTION_CURPOS:
		respawnLocation = currentLocation;
		break;
	case Protocol::RESPAWNOPTION_SPAWNPOINT:
		respawnLocation = GGameData->GetSpawnPoint(currentLocation)->spawnLocation;
		break;
	}

	{
		Protocol::GC_REVIVE res;
		target->MakePkt(*res.mutable_posdata());
		target->MakePkt(*res.mutable_statdata());
		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(res);
		targetScene->BroadCastAdjacentZone(targetZone, sendBuffer);
	}

	target->Respawn(respawnLocation);
	targetZone->GetMovingEntity().push_back(std::move(target));
}


///////////////////////////////////////////////////////
//	<	ChatRequest	>
///////////////////////////////////////////////////////
void ChatRequest::Execute(const GameSceneRef& scene, uint64 currentTick)
{
	Profile p(L"ChatRequest::Execute");
	GameSceneRef targetScene = player->GetScene();
	ZoneRef targetZone = player->GetZone();

	if (targetScene == nullptr || targetZone == nullptr)
		return;

	if (targetScene != scene)
	{
		targetScene->DoAsync(&GameScene::Request, shared_from_this());
		return;
	}

	Protocol::ChatType type = pkt.type();

	{
		Protocol::GC_CHAT res;
		res.set_name(Utils::ToString(player->GetName()));
		res.set_chat(pkt.chat());
		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(res);

		switch (type)
		{
		case Protocol::CHATTYPE_ADJACENT:
		{
			scene->BroadCastAdjacentZone(targetZone, sendBuffer, player->GetObjectId());
		}
		break;
		case Protocol::CHATTYPE_WORLD:
		{
			if (player->CanSendWorldChat(currentTick))
			{
				GWorld->BroadCast(sendBuffer, player->GetObjectId());
			}
			
		}
		break;
		}
	}
}
