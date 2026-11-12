#include "pch.h"
#include "Entity.h"

#include "StatComponent.h"

Atomic<uint64> IdGenerator::id = 0;


///////////////////////////////////////////////////////
//	<	Entity	>
///////////////////////////////////////////////////////
void Entity::Respawn(const Vector3& respawnLocation)
{
	SetLocation(respawnLocation);
	SetYaw(0);

	StatComponent* sc = GetComponent<StatComponent>();
	if (sc)
		sc->SetCurrentHP(sc->GetMaxHP());

	SetState(EntityState::Idle);
}

#pragma optimize("", off)
void Entity::MakePkt(Protocol::ObjectData& objectData)
{
	MakePkt(*objectData.mutable_summary());
	MakePkt(*objectData.mutable_pos());
	MakePkt(*objectData.mutable_stat());
}
#pragma optimize("", on)

void Entity::MakePkt(Protocol::ObjectSummaryData& summary)
{
	summary.set_objectid	(_objectId);
	summary.set_objecttype	(_type);
	summary.set_name		(Utils::ToString(_name));
	summary.set_templateid	(_templateId);
	summary.set_level		(_level);
}

void Entity::MakePkt(Protocol::ObjectPosData& pos)
{
	pos.set_objectid(_objectId);
	*pos.mutable_location() = UtilMath::ToProtoLocation(_location);
	pos.set_yaw(_yaw);
}

void Entity::MakePkt(Protocol::ObjectStatData& stat)
{
	stat.set_objectid(_objectId);
	StatComponent* sc = GetComponent<StatComponent>();
	if (sc == nullptr)
		return;
	stat.set_hp(sc->GetCurrentHP());
	stat.set_mp(sc->GetCurrentMP());
	stat.set_exp(_exp);
}
