#include "pch.h"
#include "Skill.h"

#include "MovementComponent.h"
#include "CombatComponent.h"
#include "StatComponent.h"
#include "SkillComponent.h"

#include "CombatSystem.h"
#include "GameScene.h"
#include "Zone.h"
#include "Entity.h"
#include "World.h"
#include "Player.h"


SkillInstance::SkillInstance(EntityRef owner, SkillDefinitionRef definition) : _owner(std::move(owner)), _definition(definition)
{
	_combo = -1;
	_comboEndTick = -1;
}

bool SkillInstance::AddCombo(uint64 now)
{
	if (now > _comboEndTick)
	{
		return false;
	}

	_combo++;

	if (_definition->effectFactory.size() == _combo)
	{
		return false;
	}

	_comboEndTick = now + _definition->comboWindowMsList[_combo];

	return true;
}

bool SkillInstance::Execute(Vector<EntityRef>& targets)
{
	if (_definition->effectFactory.size() <= _combo)
	{
		LOG_ERROR(L"Invalid Skill Combo SkillId: " + std::to_wstring(_definition->GetSkillId()) + L", Skill Combo: " + std::to_wstring(_combo));
		return false;
	}

	for (auto& item : _definition->effectFactory[_combo])
	{
		item->Clone()->RunEffect(_owner, targets, _definition->GetSkillId());
	}

	return true;
}

void MeleeAttackEffect::RunEffect(EntityRef owner, Vector<EntityRef>& targets, int32 skillId)
{
	if (MovementComponent* mc = owner->GetComponent<MovementComponent>())
	{
		mc->SetMoveDelayEndTick(GetTickCount64() + info.moveDelayMs);
	}

	Vector<EntityRef> finalTargets = CombatSystem::SelectTargetInAttackCone(owner, targets, info.attackRange, info.attackAngle);
	for (EntityRef finalTarget : finalTargets)
	{
		DamageEvent event = CombatSystem::MakeDamageEvent(owner, skillId, owner->GetYaw());
		if (GameSceneRef targetScene = finalTarget->GetScene())
		{
			if (info.hitDelayMs == 0 && targetScene == owner->GetScene())
			{
				Execute(targetScene.get(), event, finalTarget);
			}
			else
			{
				targetScene->DoTimer(info.hitDelayMs, [scene = targetScene, self = static_pointer_cast<MeleeAttackEffect>(shared_from_this()), event = std::move(event), targetId = finalTarget->GetObjectId()]()
					{
						self->Execute(scene.get(), std::move(event), GWorld->FindEntity(targetId));
					});
			}
		}
	}
}

void MeleeAttackEffect::Execute(GameScene* scene, DamageEvent event, const EntityRef target)
{
	if (target == nullptr)
		return;

	if (target->GetState() == EntityState::Dead)
		return;

	GameScene* newScene = target->GetScene().get();
	if (newScene != scene)
	{
		newScene->DoAsync([scene = newScene, self = static_pointer_cast<MeleeAttackEffect>(shared_from_this()), event = std::move(event), targetId = target->GetObjectId()]() {
			self->Execute(scene, event, GWorld->FindEntity(targetId));
			});
		return;
	}

	event.AddTarget(target);
	event.finalDamage = (event.rawDamage * info.attackRate * 0.01f) - event.defense;
	if (event.finalDamage < 0) event.finalDamage = 0;
	
	if (CombatSystem::ApplyDamage(target->GetZone(), event) == false)
		return;

	DamageResult result = CombatSystem::MakeDamageResult(target->GetZone(), event);

	auto fn = [](GameScene* scene, EntityRef attacker, DamageResult result) {

		if (attacker == nullptr)
			return;

		{
			Protocol::GC_DAMAGE res;
			res.set_objectid(result.targetId);
			res.set_damage(result.damage);
			res.set_isdead(result.isDead);
			SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(res);
			scene->BroadCastAdjacentZone(attacker->GetZone(), sendBuffer);
		}

		if (result.isDead)
		{
			if (PlayerRef player = static_pointer_cast<Player>(attacker))
			{
				Protocol::GC_REWARD res;
				res.set_exp(result.rewardExp);
				SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(res);
				player->GainExp(result.rewardExp);
				if (ZoneRef zone = player->GetZone())
					zone->BufferSend(sendBuffer, player);
			}
		};
		};

	EntityRef attacker = GWorld->FindEntity(event.attackerId);
	GameSceneRef attackerScene = attacker->GetScene();

	if (scene == attackerScene.get())
	{
		fn(scene, attacker, result);
	}
	else
	{
		attackerScene->DoAsync([fn, scene = attackerScene, attacker = std::move(attacker), result = std::move(result)]()
			{ fn(scene.get(), attacker, result); });
	}
}
