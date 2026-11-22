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
}

#pragma optimize("", off)
bool MovementSystem::UpdatePlayerMove(PlayerRef player, uint64 nowTick, uint64 deltaTime)
{
	if (player == nullptr)
		return false;

	if (player->GetType() == EntityState::Dead)
		return false;

	MovementComponent* mc = player->GetComponent<MovementComponent>();
	if (mc == nullptr)
		return false;


	Vector3 locationBeforeMove = player->GetLocation();
	Vector<MoveJob> pendingJobs = mc->GetPendingJob();

	bool moved = false;

	for (const MoveJob& job : pendingJobs)
	{
		player->SetYaw(job.yaw);

		if (job.velocity.IsZero())
			continue;

		if (mc->CanMove(nowTick) == false)
			break;

		const Vector3 serverLocation = player->GetLocation();
		const Vector3 nextLocation =
			GetNextPos(serverLocation, mc->GetMoveSpeed(), job, deltaTime);

		player->SetLocation(nextLocation);
		moved = true;
	}

	if (moved)
	{
		player->SetDirtyFlag(PLAYER_DIRTY_PLAYER_DETAIL);
	}

	return moved;
}



Vector3 MovementSystem::GetNextPos(const Vector3& location, float moveSpeed, const MoveJob& job, uint64 deltaTime)
{

	if (job.velocity.IsZero())
		return location;

	Vector3 Direction = UtilMath::GetMoveDirectionFromInput(job.velocity, job.yaw);
	Vector3 nextLocation = location + (Direction * moveSpeed * static_cast<float>(deltaTime) * 0.001f);
	if (GGridMap->CanGo(nextLocation) == false)
		nextLocation = location;


	Vector3 clientLocation = job.location;
	Vector3 diff = clientLocation - location;

	const float error = (clientLocation - nextLocation).Length();

	if (error > Config::Validation::MOVE_SNAP_DISTANCE)
	{
		LOG_WARN(L"Delta: " + std::to_wstring(deltaTime) + L" Diff: [" + std::to_wstring(diff.x) + L", " + std::to_wstring(diff.y) + L" Length:" + std::to_wstring(error));
		if (GServerStats)
		{
			GServerStats->game.suspiciousPacket.fetch_add(1);
		}
		return nextLocation;
	}

	return clientLocation;
}
#pragma optimize("", on)