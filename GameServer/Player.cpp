#include "pch.h"
#include "Player.h"

#include "GameData.h"
#include "StatComponent.h"
#include "SkillComponent.h"
#include "CombatComponent.h"
#include "MovementComponent.h"

bool Player::Init(PlayerSummaryData& summary, PlayerLoadData& load)
{
	PlayerData* data = GGameData->GetPlayerData(summary.level);
	if (data == nullptr)
	{
		LOG_ERROR(L"Fail to find PlayerData" + std::to_wstring(GetLevel()));
		return false;
	}

	// Entity
	SetObjectId(IdGenerator::Get());
	SetName(summary.name);
	SetLevel(summary.level);
	SetLocation(load.location);
	SetYaw(load.yaw);
	SetType(Protocol::OBJECT_CATEGORY_PLAYER);
	SetState(EntityState::Idle);
	SetTemplateId(summary.templateId);
	SetExp(load.exp);

	// Player
	_playerDBId = summary.playerId;
	_requiredExp = data->requireExp;

	// Component
	AddComponent<StatComponent>(shared_from_this())->Init(load.hp, data->maxHp, load.mp, data->maxMp);
	AddComponent<CombatComponent>(shared_from_this())->Init(data->attack, data->defense);
	MovementComponent* mc = AddComponent<MovementComponent>(shared_from_this());
	mc->Init(data->speed);
	mc->SetPendingJob(MoveJob{ GetLocation(), Vector3(), GetYaw() });

	// temp 기본 공격은 0번 스킬
	Vector<SkillState> skills;
	skills.push_back(SkillState{ 0, 1, 0 });
	AddComponent<SkillComponent>(shared_from_this())->Init(skills);
}

PlayerLoadData Player::MakeLoadData()
{
	StatComponent* sc = GetComponent<StatComponent>();
	
	PlayerLoadData data;
	data.location = GetLocation();
	data.yaw = GetYaw();
	data.hp = sc->GetCurrentHP();
	data.mp = sc->GetCurrentMP();
	data.exp = GetExp();
	
	
	return std::move(data);
}

void Player::Send(SendBufferRef& sendBuffer)
{
	if (GameSessionRef session = GetSession())
	{
		session->Send(sendBuffer);
	}
}
