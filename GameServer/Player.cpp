#include "pch.h"
#include "Player.h"

#include "GameData.h"
#include "StatComponent.h"
#include "SkillComponent.h"
#include "CombatComponent.h"
#include "MovementComponent.h"
#include "DBManager.h"
#include "job.h"

Player::~Player()
{
	//LOG_INFO(L"DeletePlayer");
}


bool Player::Init(PlayerSummaryData& summary, PlayerLoadData& load)
{
	// PlayerLevelData
	if (InitPlayerLevelData(summary.level) == false)
		return false;

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

	// Component
	MovementComponent* mc = AddComponent<MovementComponent>(shared_from_this());
	mc->SetPendingJob(MoveJob{ GetLocation(), Vector3(), GetYaw() });
	StatComponent* sc = AddComponent<StatComponent>(shared_from_this());
	sc->SetCurrentHP(load.hp);
	sc->SetCurrentMP(load.mp);

	// temp 기본 공격은 0번 스킬
	Vector<SkillState> skills;
	skills.push_back(SkillState{ 0, 1, 0 });
	AddComponent<SkillComponent>(shared_from_this())->Init(skills);

	return true;
}

void Player::Respawn(const Vector3& respawnLocation)
{
	Entity::Respawn(respawnLocation);

	StatComponent* sc = GetComponent<StatComponent>();
	sc->SetCurrentMP(sc->GetMaxMP());

	MovementComponent* mc = AddComponent<MovementComponent>(shared_from_this());
	mc->SetPendingJob(MoveJob{ respawnLocation, Vector3(), GetYaw() });
}

void Player::GainExp(int32 exp)
{
	WRITE_LOCK;
	int32 currentExp = GetExp();
	currentExp += exp;

	int32 requiredExp = GetRequiredExp();

	if (currentExp >= requiredExp)
	{
		currentExp -= requiredExp;
		SetExp(currentExp);
		LevelUp();
	}
}

void Player::LevelUp()
{
	int32 currentLevel = GetLevel();
	SetLevel(currentLevel + 1);
	
	InitPlayerLevelData(GetLevel());

	StatComponent* sc = GetComponent<StatComponent>();

	GDBManager->DoAsync(Job([player = static_pointer_cast<Player>(shared_from_this())]() 
		{
			GDBManager->SaveLevelUp(player->GetDBId(), player->GetLevel());
			{
				Protocol::GC_LEVELUP pkt;
				player->MakePkt(*pkt.mutable_data());
				SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
				player->Send(sendBuffer);
			}
		}));
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

void Player::Send(const SendBufferRef& sendBuffer)
{
	if (GameSessionRef session = GetSession())
	{
		session->Send(sendBuffer);
	}
}

bool Player::InitPlayerLevelData(int32 level)
{
	PlayerData* data = GGameData->GetPlayerData(level);
	if (data == nullptr)
	{
		LOG_ERROR(L"Fail to find PlayerData" + std::to_wstring(GetLevel()));
		return false;
	}

	// player
	_requiredExp = data->requireExp;

	//Component
	AddComponent<StatComponent>(shared_from_this())->Init(data->maxHp, data->maxHp, data->maxMp, data->maxMp);
	AddComponent<CombatComponent>(shared_from_this())->Init(data->attack, data->defense);
	MovementComponent* mc = AddComponent<MovementComponent>(shared_from_this());
	mc->Init(data->speed);

	return true;
}
