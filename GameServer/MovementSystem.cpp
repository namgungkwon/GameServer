#include "pch.h"
#include "MovementSystem.h"
#include "zone.h"
#include "MovementComponent.h"
#include "GridMap.h"

#include "entity.h"
#include "Player.h"


///////////////////////////////////////////////////////
//	<	MovementSystem	>
///////////////////////////////////////////////////////
void MovementSystem::Update(ZoneRef zone, uint64 nowTick, uint64 deltaTime, OUT Vector<EntityRef>& movingPlayer)
{
	PROFILE_BEGIN(L"MovementSystem::Update");
	if (zone == nullptr)
		return;

	for (auto& [id, player] : zone->GetPlayers())
	{
		if (UpdatePlayerMove(player, nowTick, deltaTime))
		{
			if (player->ShouldSendMovePacket(nowTick))
				movingPlayer.push_back(player);
		}
	}
	PROFILE_END(L"MovementSystem::Update");
}

bool MovementSystem::UpdatePlayerMove(PlayerRef player, uint64 nowTick, uint64 deltaTime)
{
	if (player == nullptr)
		return false;

	if (player->GetType() == EntityState::Dead)
		return false;

	MovementComponent* mc = player->GetComponent<MovementComponent>();
	if (mc == nullptr)
		return false;

	MoveJob& job = mc->GetPendingJob();
	if (job.velocity.IsZero() && player->GetYaw() == job.yaw)
		return false;

	player->SetYaw(job.yaw);

	if (mc->CanMove(nowTick) == false)
		return false;

	Vector3 nextLocation = GetNextPos(mc, job, deltaTime);
	player->SetLocation(nextLocation);
	return true;
}

Vector3 MovementSystem::GetNextPos(MovementComponent* mc, MoveJob& job, uint64 deltaTime)
{
	if (job.velocity.IsZero())
	{
		return job.location;
	}

	Vector3 location = job.location;
	Vector3 Direction = UtilMath::GetMoveDirectionFromInput(job.velocity, job.yaw);

	Vector3 nextLocation = location + (Direction * mc->GetMoveSpeed() * static_cast<float>(deltaTime) * 0.001f /* ms 변환 */);

	if (GGridMap->CanGo(nextLocation) == false)
		nextLocation = location;

	return nextLocation;
}