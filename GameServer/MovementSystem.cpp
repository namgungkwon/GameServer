#include "pch.h"
#include "MovementSystem.h"
#include "zone.h"
#include "MovementComponent.h"
#include "GridMap.h"

#include "entity.h"
#include "Player.h"

void MovementSystem::Update(ZoneRef zone, uint64 nowTick, uint64 deltaTime, OUT Vector<EntityRef>& movingPlayer)
{
	if (zone == nullptr)
		return;

	for (auto& [id, player] : zone->GetPlayers())
	{
		if (UpdateEntityMove(player, nowTick, deltaTime))
			movingPlayer.push_back(player);
	}
}

bool MovementSystem::UpdateEntityMove(EntityRef entity, uint64 nowTick, uint64 deltaTime)
{
	if (entity == nullptr)
		return false;

	if (entity->GetType() == EntityState::Dead)
		return false;

	MovementComponent* mc = entity->GetComponent<MovementComponent>();
	if (mc == nullptr)
		return false;

	MoveJob job = mc->GetPendingJob();
	if (job.velocity.IsZero() && entity->GetYaw() == job.yaw)
		return false;

	entity->SetYaw(job.yaw);

	if (mc->CanMove(nowTick) == false)
		return false;

	Vector3 nextLocation = GetNextPos(entity, job, deltaTime);
	entity->SetLocation(nextLocation);
	return true;
}

Vector3 MovementSystem::GetNextPos(EntityRef entity, MoveJob& job, uint64 deltaTime)
{
	MovementComponent* mc = entity->GetComponent<MovementComponent>();

	if (job.velocity.IsZero())
	{
		return job.location;
	}

	Vector3 location = job.location;
	Vector3 Direction = UtilMath::GetMoveDirectionFromInput(job.velocity, job.yaw);

	Vector3 nextLocation = location + (Direction * mc->GetMoveSpeed() * deltaTime * 0.001f /* ms 변환 */);

	if (GGridMap->CanGo(nextLocation) == false)
		nextLocation = location;

	return nextLocation;
}