#include "pch.h"
#include "Player.h"

#include "GameData.h"
#include "StatComponent.h"
#include "SkillComponent.h"
#include "CombatComponent.h"
#include "MovementComponent.h"

bool Player::Init(PlayerSummaryData& summary, PlayerLoadData& load)
{
	PlayerData* data = GGameData->GetPlayerData(GetLevel());
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
	SetType(EntityType::PLAYER);
	SetState(EntityState::Idle);

	// Player
	_dbId = summary.dbId;
	_templateId = summary.templateId;
	_exp = load.exp;
	_requiredExp = data->requireExp;

	// Component
	AddComponent<StatComponent>(shared_from_this())->Init(load.hp, data->maxHp, load.mp, data->maxMp);
	AddComponent<CombatComponent>(shared_from_this())->Init(data->attack, data->defense);
	AddComponent<MovementComponent>(shared_from_this())->Init(data->speed);

	// temp 기본 공격은 0번 스킬
	Vector<SkillState> skills;
	skills.push_back(SkillState{ 0, 1, 0 });
	AddComponent<SkillComponent>(shared_from_this())->Init(skills);
}

void Player::MakePkt(Protocol::GC_ENTER_GAME& pkt)
{
	// summary
	Protocol::PlayerSummaryData* summary = pkt.mutable_summarydata();
	summary->set_objectid(GetObjectId());
	summary->set_name(Utils::ToString(GetName()));
	summary->set_templateid(_templateId);

	// pos
	Protocol::PlayerPosData* pos = pkt.mutable_posdata();
	Protocol::Vector3* vec = pos->mutable_location();
	vec->set_x(GetLocation().x);
	vec->set_y(GetLocation().y);
	vec->set_z(GetLocation().z);
	pos->set_yaw(GetYaw());

	// stat
	StatComponent* sc = GetComponent<StatComponent>();
	Protocol::PlayerStatData* stat = pkt.mutable_statdata();
	stat->set_hp(sc->GetCurrentHP());
	stat->set_mp(sc->GetCurrentMP());
	stat->set_exp(_exp);
}
