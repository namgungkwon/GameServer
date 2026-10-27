#include "pch.h"
#include "AISystem.h"

#include "Zone.h"
#include "Monster.h"
#include "AIComponent.h"
#include "MovementComponent.h"

void AISystem::Update(ZoneRef zone, uint64 nowTick, uint64 deltaTime, OUT Vector<EntityRef>& movingEntity)
{
	for (auto& [id, monster] : zone->GetMonsters())
	{
		AIComponent* ai = monster->GetComponent<AIComponent>();
		if (ai == nullptr)
			return;
		ai->Update(nowTick, deltaTime);

		if (ai->GetPath().size() != 0)
		{
			if (UpdateAIMove(monster, nowTick, deltaTime))
				movingEntity.push_back(monster);
		}
	}
}

bool AISystem::UpdateAIMove(EntityRef entity, uint64 nowTick, uint64 deltaTIme)
{
	if (entity == nullptr)
		return false;

	if (entity->GetState() == EntityState::Dead)
		return false;

	AIComponent* ai = entity->GetComponent<AIComponent>();
	MovementComponent* mc = entity->GetComponent<MovementComponent>();

	if (ai == nullptr || mc == nullptr)
		return false;

	if (mc->CanMove(nowTick) == false)
		return false;

	Vector3 location = entity->GetLocation();
	Vector<Vector3> path = ai->GetPath();
	int32 pathIndex = ai->GetPathIndex();


	if (location.DistanceSq(path[pathIndex]) <= 0.1f)
		pathIndex++;

	if (path.size() <= pathIndex)
	{
		ai->ResetPath();
		return false;
	}

	Vector3 nextPoint = path[pathIndex];

	float maxDistance = UtilMath::Min(location.Distance(nextPoint), mc->GetMoveSpeed() * deltaTIme * 0.001f);

	Vector3 dirVector = nextPoint - location;
	Vector3 normalVector = dirVector.Normalized();
	Vector3 nextLocation = location + (normalVector * maxDistance);

	entity->SetYaw(UtilMath::GetYawFromVector(dirVector));
	entity->SetLocation(nextLocation);
	ai->SetPathIndex(pathIndex);

	return true;
}

