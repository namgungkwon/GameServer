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
	Protocol::ObjectPosData posData = pkt.posdata();
	attacker->SetLocation(UtilMath::ToServerLocation(posData.location()));
	attacker->SetYaw(posData.yaw());

	GameSceneRef playerScene = attacker->GetScene();

	if (playerScene != scene)
	{
		playerScene->DoAsync(&GameScene::Request, shared_from_this());
		return;
	}

	if (attacker->GetState() == EntityState::Dead)
		return;

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
			res.set_isdead(target->GetState() == EntityState::Dead);
			SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(res);
			scene->BroadCastAdjacentZone(attacker->GetZone(), sendBuffer);
		}
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