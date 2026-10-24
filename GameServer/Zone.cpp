#include "pch.h"
#include "Zone.h"

#include "entity.h"

void Zone::EnterZone(EntityRef entity)
{
	if (entity == nullptr)
		return;

	uint64 objectId = entity->GetObjectId();

	switch (entity->GetType())
	{
	case EntityType::PLAYER:
	{
		PlayerRef player = static_pointer_cast<Player>(entity);
		_players[objectId] = player;
		break;
	}
	// Todo 
	// 타입 추가
	default:
		break;
	}
}

void Zone::LeaveZone(EntityRef entity)
{
	if (entity == nullptr)
		return;

	uint64 objectId = entity->GetObjectId();

	switch (entity->GetType())
	{
	case EntityType::PLAYER:
	{
		_players.erase(objectId);
		break;
	}
	// Todo 
	// 타입 추가
	default:
		break;
	}
}
