#include "pch.h"
#include "SceneRequest.h"

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

void AttackRequest::Execute(GameSceneRef scene, uint64 currentTick)
{
	if (attacker->GetState() == EntityState::Dead)
		return;

	GameSceneRef playerScene = attacker->GetScene();

	if (playerScene != scene)
	{
		playerScene->DoAsync(&GameScene::Request, shared_from_this());
		return;
	}

	Protocol::ObjectPosData posData = pkt.posdata();
	attacker->SetLocation(UtilMath::ToServerLocation(posData.location()));
	attacker->SetYaw(posData.yaw());

	CombatComponent* aCombat = attacker->GetComponent<CombatComponent>();
	SkillComponent* aSkill = attacker->GetComponent<SkillComponent>();
	StatComponent* aStat = attacker->GetComponent<StatComponent>();
	MovementComponent* aMovement = attacker->GetComponent<MovementComponent>();

	if (aCombat == nullptr || aSkill == nullptr || aStat == nullptr)
		return;

	const uint64 skillId = pkt.skillid();

	if (aSkill->HasSkill(skillId) == false)
		return;

	if (aSkill->CooldownReady(skillId, currentTick) == false)
		return;

	const uint64 moveDelay = aSkill->UseSkill(skillId, currentTick);
	aMovement->SetMoveDelayEndTick(currentTick + moveDelay);

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
			targets.push_back(entity);
	}

	SkillData* skill = aSkill->GetSkill(skillId);
	Vector<EntityRef> finalTargets = CombatSystem::SelectTargetInAttackCone(attacker, targets, skill->attackRange, skill->attackAngle);


	auto fn = [](GameSceneRef scene, ZoneRef zone, DamageEvent event, EntityRef attacker, EntityRef target) {
		if (CombatSystem::ApplyDamage(zone, event) == false)
			return;

		DamageResult result = CombatSystem::MakeDamageResult(zone, event);

		{
			Protocol::GC_DAMAGE res;
			res.set_objectid(target->GetObjectId());
			res.set_damage(result.damage);
			res.set_isdead(result.isDead);
			SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(res);
			scene->BroadCastAdjacentZone(attacker->GetZone(), sendBuffer);
		}

		if (result.isDead)
		{
			if (attacker->GetType() == Protocol::OBJECT_CATEGORY_PLAYER &&
				target->GetType() == Protocol::OBJECT_CATEGORY_MONSTER)
			{
				PlayerRef player = static_pointer_cast<Player>(attacker);
				{
					Protocol::GC_REWARD res;
					res.set_exp(target->GetExp());
					SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(res);
					player->Send(sendBuffer);
				}
				player->GainExp(target->GetExp());
			}
		};
	};


	for (EntityRef finalTarget : finalTargets)
	{
		LOG_INFO(L"AttackSuccess!");
		GameSceneRef targetScene = finalTarget->GetScene();
		ZoneRef targetZone = finalTarget->GetZone();
		DamageEvent event = CombatSystem::MakeDamageEvent(attacker, finalTarget, skillId, attacker->GetYaw());

		if (targetScene == playerScene)
		{
			fn(targetScene, targetZone, event, attacker, finalTarget);
		}
		else
		{
			EntityRef attacker = attacker;
			targetScene->DoAsync([fn, targetScene, targetZone, event, attacker, finalTarget]()
				{ fn(targetScene, targetZone, event, attacker, finalTarget); });
		}
	}
}

void ReviveRequest::Execute(GameSceneRef scene, uint64 currentTick)
{
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

	targetZone->GetMovingEntity().push_back(target);
	target->Respawn(respawnLocation);

	{
		Protocol::GC_REVIVE res;
		target->MakePkt(*res.mutable_posdata());
		target->MakePkt(*res.mutable_statdata());
		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(res);
		targetScene->BroadCastAdjacentZone(targetZone, sendBuffer);
	}
}
