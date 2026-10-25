#include "pch.h"
#include "MovementSystem.h"
#include "zone.h"
#include "MovementComponent.h"
#include "entity.h"
#include "GridMap.h"

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
	entity->SetYaw(job.yaw);

	if (mc->CanMove(nowTick) == false)
		return false;

	if (job.velocity.IsZero())
		return false;

	Vector3 nextLocation = GetNextPos(entity, job, deltaTime);
	LOG_INFO(L"nextLocation" + std::to_wstring(nextLocation.x) + L'/' + std::to_wstring(nextLocation.y) + L'/' + std::to_wstring(nextLocation.z));
	LOG_INFO(L"DeltaTime");
	entity->SetLocation(nextLocation);
	return true;
}

Vector3 MovementSystem::GetNextPos(EntityRef entity, MoveJob& job, uint64 deltaTime)
{
	MovementComponent* mc = entity->GetComponent<MovementComponent>();
	
	Vector3 location = job.location;
	Vector3 normal = job.velocity.Normalized();

	Vector3 nextLocation = location + (normal * mc->GetMoveSpeed() * deltaTime * 0.001f /* ms 변환 */);

	if (GGridMap->CanGo(nextLocation) == false)
		nextLocation = location;

	return nextLocation;
}