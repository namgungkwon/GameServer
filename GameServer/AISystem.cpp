#include "pch.h"
#include "AISystem.h"

#include "Zone.h"
#include "Monster.h"
#include "MovementComponent.h"


///////////////////////////////////////////////////////
//	<	AISystem	>
///////////////////////////////////////////////////////
void AISystem::Update(ZoneRef zone, uint64 nowTick, uint64 deltaTime, OUT Vector<EntityRef>& movingEntity)
{
	for (auto& [id, monster] : zone->GetMonsters())
	{
		monster->Update(nowTick, deltaTime);

		if (monster->GetPath().size() != 0)
		{
			if (UpdateAIMove(monster, nowTick, deltaTime))
				movingEntity.push_back(monster);
		}
	}
}

bool AISystem::UpdateAIMove(NPCRef npc, uint64 nowTick, uint64 deltaTIme)
{
	if (npc == nullptr)
		return false;

	if (npc->GetState() == EntityState::Dead)
		return false;

	MovementComponent* mc = npc->GetComponent<MovementComponent>();

	if (mc == nullptr)
		return false;

	if (mc->CanMove(nowTick) == false)
		return false;

	Vector3 location = npc->GetLocation();
	Vector<Vector3>& path = npc->GetPath();
	int32 pathIndex = npc->GetPathIndex();


	if (location.DistanceSq(path[pathIndex]) <= 0.1f)
		pathIndex++;

	if (path.size() <= pathIndex)
	{
		npc->ResetPath();
		return false;
	}

	Vector3 nextPoint = path[pathIndex];

	float maxDistance = UtilMath::Min(location.Distance(nextPoint), mc->GetMoveSpeed() * deltaTIme * 0.001f);

	Vector3 dirVector = nextPoint - location;
	Vector3 normalVector = dirVector.Normalized();
	Vector3 nextLocation = location + (normalVector * maxDistance);

	npc->SetYaw(UtilMath::GetYawFromVector(dirVector));
	npc->SetLocation(nextLocation);
	npc->SetPathIndex(pathIndex);

	return true;
}

