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
	player->SetLocation(UtilMath::ToServerLocation(posData.location()));
	player->SetYaw(posData.yaw());

	GameSceneRef playerScene = player->GetScene();

	if (playerScene != scene)
	{
		playerScene->DoAsync(&GameScene::Request, shared_from_this());
		return;
	}

	if (player->GetState() == EntityState::Dead)
		return;

	CombatComponent* aCombat = player->GetComponent<CombatComponent>();
	SkillComponent* aSkill = player->GetComponent<SkillComponent>();
	StatComponent* aStat = player->GetComponent<StatComponent>();
	MovementComponent* aMovement = player->GetComponent<MovementComponent>();

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
		player->MakePkt(*res.mutable_posdata());
		res.set_skillid(skillId);
		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(res);
		scene->BroadCastAdjacentZone(player->GetZone(), sendBuffer, player->GetObjectId());
	}

	if (pkt.targets().size() == 0)
		return;

	// 타겟 선별
	Vector<EntityRef> targets;
	for (uint64 targetId : pkt.targets())
	{
		MonsterRef monster = GWorld->FindMonster(targetId);
		targets.push_back(static_pointer_cast<Entity>(monster));
	}

	SkillData* skill = aSkill->GetSkill(skillId);
	Vector<EntityRef> finalTargets = CombatSystem::SelectTargetInAttackCone(player, targets, skill->attackRange, skill->attackAngle);


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
		DamageEvent event = CombatSystem::MakeDamageEvent(player, finalTarget, skillId, player->GetYaw());

		if (targetScene == playerScene)
		{
			fn(targetScene, targetZone, event, player, finalTarget);
		}
		else
		{
			PlayerRef attacker = player;
			targetScene->DoAsync([fn, targetScene, targetZone, event, attacker, finalTarget]()
				{ fn(targetScene, targetZone, event, attacker, finalTarget); });
		}
	}
}