#include "pch.h"
#include "AISystem.h"

#include "Zone.h"
#include "Monster.h"
#include "AIComponent.h"

void AISystem::Update(ZoneRef zone, uint64 nowTick, uint64 deltaTime, OUT Vector<EntityRef>& movingEntity)
{
	for (auto& [id, monster] : zone->GetMonsters())
	{
		if (monster->GetState() == EntityState::Chase)
		{
			movingEntity.push_back(monster);
		}

		AIComponent* ai = monster->GetComponent<AIComponent>();
		ai->Update(nowTick, deltaTime);
	}
}