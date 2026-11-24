# 이동 프로세스에 대한 분석

## 개요
 본 포트폴리오의 MovementSystem의 경우 Claude의 도움을 받아 작성하였습니다.
 이에 Claude가 제공한 코드를 온전히 제 것으로 하기 위해서, 앞으로의 최적화화 코드 확장을 용이하게 하기 위해서 코드를 분석하였습니다.

- Claude 코드가 직접 파일을 건드리는 기능을 막고, 제공한 코드를 이해하고 붙여 넣었습니다.

## 전체 동작 프로세스
1. 클라이언트 입력 -> ClientPacketHandler::CG_MOVE() -> Player->PushPendingJob() 으로 Job을 누적
2. GameScene::Update() -> MovementSystem::UpdatePlayerMove() 에서 이동 갱신 진행
3. 서버 응답
   - 정상 이동 : GameScene::Update -> Zone->ClearPendingJob() 에서 Move or Spawn or Despawn(Zone 이동 시) 처리
   - 비정상 이동 : MovementSystem::NotifyCorrection() 에서 보정 패킷 처리

## 기존 코드와 한계

### 기존 코드
```c++
//MoveJob
//- sendClientTick : 클라이언트가 이동 상태를 보고한 시각
//- location       : 클라이언트가 보고한 위치
//- velocity       : 클라이언트 입력 기반 이동 방향/세기
//- yaw            : 클라이언트가 보고한 시야 방향


bool MovementSystem::UpdatePlayerMove(PlayerRef player, uint64 nowTick, uint64 deltaTime)
{
	if (player == nullptr)
		return false;

	if (player->GetState() == EntityState::Dead)
		return false;

	MovementComponent* mc = player->GetComponent<MovementComponent>(); // 이동할 수 없는 오브젝트 일 경우
	if (mc == nullptr)
		return false;


	Vector3 locationBeforeMove = player->GetLocation();
	Vector<MoveJob> pendingJobs = mc->GetPendingJob(); // 항상 1개의 가장 최신의 Job을 보유하고 있음

	bool moved = false;

  size_t size = pendingJobs.size();
	for (size_t i = 1; i < size; ++i)
	{
    MoveJob from = pendingJobs[i - 1]; // i - 1 번째 Job을 처리하게 됨
    MoveJob to = pendingJobs[i];

		player->SetYaw(job.yaw);

		if (from.velocity.IsZero()) // 이동을 하지 않는 Job의 경우 처리하지 않음
			continue;

		if (mc->CanMove(nowTick) == false) // 이동할 수 없는 상황인 경우(공격 등)
			break;

		const Vector3 serverLocation = player->GetLocation();
		const Vector3 nextLocation =
			GetNextPos(serverLocation, mc->GetMoveSpeed(), job, to.sendClientTick - from.sendClientTick); // 다음 좌표를 가져온다. tick은 클라이언트가 보낸 값의 차를 delta로 계산

		player->SetLocation(nextLocation);
		moved = true;
	}

	if (moved)
	{
		player->SetDirtyFlag(PLAYER_DIRTY_PLAYER_DETAIL); // 이동했을 경우 다음의 DB 업데이트에 반영할 수 있게 플래그 설정
	}

	return moved;
}



Vector3 MovementSystem::GetNextPos(const Vector3& location, float moveSpeed, const MoveJob& job, uint64 deltaTime)
{
	if (job.velocity.IsZero()) // 이동하지 않을 경우 현재 위치 반환
		return location;

	Vector3 Direction = UtilMath::GetMoveDirectionFromInput(job.velocity, job.yaw); // 대상의 현재 yaw값과 velocity 값으로 절대 좌표의 방향을 가져옴
	Vector3 nextLocation = location + (Direction * moveSpeed * static_cast<float>(deltaTime) * 0.001f); // 다음 위치 계산
	if (GGridMap->CanGo(nextLocation) == false) // 다음위치에 갈 수 없는 경우 현재 위치를 설정
		nextLocation = location;


	Vector3 clientLocation = job.location;
	Vector3 diff = clientLocation - location; // 클라이언트와 현재 좌표의 차이를 계산

	const float error = (clientLocation - nextLocation).Length(); // 오차 산출

	if (error > Config::Validation::MOVE_SNAP_DISTANCE) // 오차가 지정한 거리보다 멀면
	{
		LOG_WARN(L"Delta: " + std::to_wstring(deltaTime) + L" Diff: [" + std::to_wstring(diff.x) + L", " + std::to_wstring(diff.y) + L" Length:" + std::to_wstring(error));
		if (GServerStats)
		{
			GServerStats->game.suspiciousPacket.fetch_add(1);
		}
		return nextLocation; // 서버에서 계산한 다음 위치를 반환
	}

	return clientLocation; // 허용 오차 내 클라이언트 좌표를 수용
}
```
### 기존 구현
 - MoveJob의 두 구간 from과 to의 clienttick을 기준으로 deltaTime을 산정하였고, from에서 velocity와 location을 이용하여 다음 위치를 계산하는 방식으로 진행하였습니다.
 - Client는 MovePkt을 1. 이동 중일 때 일정 간격마다 2. 방향이 바뀔 때 3. 정지할 때 패킷을 보내기 때문에, 구조 상 가장 최근의 MoveJob은 처리되지 않고 다음 Job이 들어 온 이후에 처리가 되게 구현하였습니다.

### 코드의 오류
 - GetNextPos에서 diff = clientLocation - location; 으로 여기서 location의 경우 이동 처리된 값을 가지기 때문에, movespeed * deltaTime의 오차를 항상 가지고 있음.

### 기존 한계
 - 클라이언트 지연으로 인해서(또는 공격자의 조작으로) clientTick이 정해진 범주를 벗어 날 경우,  정해진 지형을 벗어나서 움직이는 현상이 발생할 수 있음.
 - 클라이언트가 부활 등으로 좌표가 완전이 이동 될 때에, 이전 이동 패킷이 남아있을 경우, 서버와의 거리 오차로 인해 suspiciousPacket으로 분류되어 비정상 클라이언트로 분류될 수 있음.
 - deltaTime을 ClientTick의 값 비교만 하고, 최대 값을 제한하지 않기 때문에 clientTick 조작으로 비정상적으로 긴 이동 시간을 확보할 수 있음


## 적용 파일
 - [GameServer/GameConfig.h](https://github.com/namgungkwon/GameServer/blob/main/GameServer/GameConfig.h) - 이동 처리에 사용되는 상수들을 추가
 - [GameServer/MovementComponent.h](https://github.com/namgungkwon/GameServer/blob/main/GameServer/MovementComponent.h) - 이동 상태 검증용 구조체 MoveSyncState 추가
 - [GameServer/MovementComponent.cpp](https://github.com/namgungkwon/GameServer/blob/main/GameServer/MovementComponent.cpp) - PendingJob의 최대치 초과 시 처리
 - [GameServer/MovementSystem.h](https://github.com/namgungkwon/GameServer/blob/main/GameServer/MovementSystem.h) - 로그 기록용 구조체 Violation, 이동 결과 반환 구조체 MoveResult 추가
 - [GameServer/MovementSystem.cpp](https://github.com/namgungkwon/GameServer/blob/main/GameServer/MovementSystem.cpp) - 위치보정 후 동기화 대기, 서버지연 시 최대 처리시간 제한, 비정상적인 위치 점프(teleport) 및 연속성 위반 방어 방어, 속도 핵 제어 등 추가
 - [GameServer/SceneRequest.cpp](https://github.com/namgungkwon/GameServer/blob/main/GameServer/SceneRequest.cpp) - AttackRequest부분에서 아직 처리 되지 않은 거리만큼 여유거리를 계산

## 추가 로직 설명

### >> GameConfig
| 상수 명 | 기능 | 비고 |
|---|---|---|
| MOVE_MAX_JOB_DELTA_MS | 클라이언트의 각 이동 패킷 Term 의 최대 크기 제한 | 공격자가 임의로 Tick을 크게 벌려서 보내는 스피드 핵 방지 + 클라이언트 프레임 끊김 대응 |
| MOVE_TIME_BUDGET_MAX_MS | budget 변수의 최댓값 제한(budget변수는 매 ServerTick마다 DeltaTime 만큼 증가하며, JOB_DELTA_MS 만큼 소모) | 네트워크 지연으로 패킷이 몰려왔을 때 처리량 제한 |
| MOVE_SPEED_TOLERANCE | 최대 이동거리(EntitySpeed + deltaTime)의 상대적인 여유 값(정상 클라이언트가 처리될 수 있는 여유값) | 예측/보정 시 오차 |
| MOVE_REACH_EPSILON | 이동거리가 매우 짧을 때 상대여유 값으로 처리되지 않는 절대 여유값 | 매우 짧은구간 이동 시 여유 오차 |
| MOVE_SNAP_DISTANCE | 서버와 클라이언트의 좌표 차이의 허용 값(이 값을 초과 시 클라이언트를 서버위치로 스냅) | 연속성 위반 방어 + 클라 위치보정 |
| MOVE_FROZEN_LIMIT_MULT | 스킬 후딜 직후 이동 패킷의 여유량 증가 배율 | 후딩때는 클라이언트의 이동 패킷을 처리하지 않으므로, 오차 값에 대한 여유를 둠 |
| MOVE_RESYNC_ACK_DISTANCESQ | 서버가 보정 패킷을 보낸 이후 보정 된 값에서 오는 패킷 확인 거리 | 보정 위치에서 유효한 첫 보고 이전의 패킷을 폐기 |
| MOVE_RESYNC_TIMEOUT_MS | 클라이언트가 보정 위치 적용을 하지 않을경우 보정 패킷을 다시 보내는 간격 | 네트워크 패킷 누락 대비 |

- MOVE_SPEED_TOLERANCE와 MOVE_REACH_EPSILON은 이동거리가 짧을 경우 EPSILON이 길 경우 TOLERANCE가 오차 값을 제어

### >> MovementComponent.h
```c++
// 이동 검증 상태 (게임 틱 스레드에서만 접근)
struct MoveSyncState
{
  // ResetBase() 를 호출해서 GC_MOVE_CORRECTION패킷을 송신하여 Entity의 위치를 순간이동 시켰을 때 관련된 변수
	bool	awaitingResync = false; // ResetBase() 호출 시 true, 클라이언트의 CG_MOVE_CORRECTION 을 응답받을 경우 false로 변경
	uint64	resyncStartTick = 0; // ResetBase() 호출 시 틱 설정, awatingResync가 true일 경우, 해당 틱 이전의 패킷을 폐기

  // Entity가 이동할 수 없는 경우 true 변경
	bool	frozen = false; // 서버에서는 바로 이동할 수 없지만, 클라이언트는 후딜 직전까지 이동하기 때문에 frozen이 풀린 후 이동 여유를 더 주기 위해 사용하는 변수

	int64	timeBudgetMs = 1000; // 서버의 tick 만큼 증가하며, 클라이언트 이동 패킷의 이동시간만큼 감소
	uint64	budgetServerTick = 0; // 서버의 현재 tick을 계산하기 위해서 저장하는 이전 tick 시간
};
```
- 코드 내의 설명 참조
- awaitingResync, resyncStartTick - Entity 순간이동 시 이전패킷 폐기용
- frozen - Entity 이동 불가 시 직후 이동패킷 판단용
- timebudgetMs, budgetserverTick - 클라이언트 clientTick 조작 방지용


### >>> MovementComponent.cpp
```C++
void MovementComponent::PushPendingJob(MoveJob job)
{
	WRITE_LOCK;

	if (job.sendClientTick < _pendingJob.back().sendClientTick)
		return;

	if (_pendingJob.size() >= MAX_PENDING_JOBS) // pendingJobs가 최대 크기를 넘었을 경우
		_pendingJob.erase(_pendingJob.begin() + 1); // 가장 오래된 패킷(이전tick 패킷 제외) 을 제거

	_pendingJob.push_back(std::move(job)); // 복사비용을 없앰
}
```
- pendingJob의 Max를 지정하여 네트워크 지연이나 servertick 지연이 되어 pendingqueue가 무한히 쌓이는 일 방지
  -> 이건 생각해봤는데, 현재 Client가 200ms 로 패킷을 보내고, Server는 50ms 로 패킷을 처리하므로 일반적인 상황에서 5개 이상 쌀이는 일이 없을 것으로 판단된다.

### >>> MovementSystem.h
```C++
struct Violation
{
	const wchar_t* reason = nullptr; // 위반 사유
	size_t			segment = 0; // MoveJobs의 i번째 구간(1일 경우 첫 구간)
	float			claimed = 0.f; // 위반 사유에 따른 위반 수치
	float			allowed = 0.f; // 위반 사유의 허용 수치
	uint64			dtMs = 0; // 클라이언트가 이동했다고 주장하는 시간
	Vector3			serverLocation; // 서버 좌표
	Vector3			clientLocation; // 클라이언트 좌표
	bool			isSpeed = false;	// 속도 위반(의심 패킷)인지
};

	struct MoveResult
	{
		bool changed = false;	// 좌표 또는 yaw가 바뀌었는지
		bool settled = false;	// 마지막으로 처리한 job이 정지 상태인지
	};
```
- 기존에 bool 값으로 이동했는지 여부만 반환했었지만, 마지막 이동이 정지상태인지 확인하는 변수가 추가되어 MoveResult로 구현 되었음

### >>> MoveSystem.cpp
``` C++
MovementSystem::MoveResult MovementSystem::ProcessPlayerMove(const PlayerRef& player)
{
	MoveResult result;

	if (player == nullptr)
		return result;

	MovementComponent* mc = player->GetComponent<MovementComponent>();
	if (mc == nullptr)
		return result;

	MoveSyncState& sync = mc->GetSyncState();

	// 죽은 동안 입력 Job이 쌓이는 걸 방지하기 위해서 큐를 비운다.
	const Vector<MoveJob> rawJobs = mc->GetPendingJob();

	if (player->GetState() == EntityState::Dead)
		return result;

	// tick이 증가하는 보고만 사용한다. (tick 감소 패킷은 deltaTime때에 uint64 언더플로가 되어 매우 큰 양수가 됨)
	Vector<MoveJob> jobs;
	jobs.reserve(rawJobs.size());
	for (const MoveJob& job : rawJobs)
	{
		if (jobs.empty() == false && job.sendClientTick <= jobs.back().sendClientTick)
			continue;

		jobs.push_back(job);
	}

	// [0]은 기준 job. 새 보고가 없으면 처리할 구간이 없다.
	if (jobs.size() < 2)
		return result;

	// 시간 기준은 실제 시간을 쓴다. (Scene 틱 시각은 밀린 만큼 뒤처질 수 있고, 스킬 후딜도 GetTickCount64 기준)
	const uint64 now = ::GetTickCount64();

	// ---------------------------------------------------------------
	// ---------------------------------------------------------------
  // 본격 이동 처리
	// ---------------------------------------------------------------
	// ---------------------------------------------------------------
	// [1] 재동기화 대기: 보정 위치에서 보낸 첫 보고(ack)가 올 때까지 옛 보고를 버린다.
	// ---------------------------------------------------------------
	if (sync.awaitingResync)
	{
		const Vector3 serverLocation = player->GetLocation();

		size_t k = 0;
		while (k < jobs.size() && serverLocation.Distance2DSq(jobs[k].location) > Config::Validation::MOVE_RESYNC_ACK_DISTANCESQ) // 보정 후 위치와 거리가 먼 Job을 선정
			++k;

		if (k == jobs.size()) // 아직 모든 패킷이 보정업데이트가 되어있지  않을경우
		{
			// 주기적으로 다시 보낸다.
			if (now - sync.resyncStartTick > Config::Validation::MOVE_RESYNC_TIMEOUT_MS)
			{
				sync.resyncStartTick = now;
				NotifyCorrection(player);
			}
			return result;
		}

    // 보정이 완료된 패킷이 존재하므로, Resync상태를 false로 바꾸고, 보정 이전의 패킷을 제거
		sync.awaitingResync = false;
		jobs.erase(jobs.begin(), jobs.begin() + k);

		if (jobs.size() < 2)
			return result;
	}

	// ---------------------------------------------------------------
	// [2] 시간 예산: 서버의 실제 경과 시간만큼 쌓이고(상한 있음), 이동 구간에서 소모된다.
	// ---------------------------------------------------------------
	const int64 maxBudget = Config::Validation::MOVE_TIME_BUDGET_MAX_MS;
	const uint64 elapsed = (now > sync.budgetServerTick) ? (now - sync.budgetServerTick) : 0; // 마지막 갱신 후 흐른 시간
	sync.budgetServerTick = now;

	int64 budget = UtilMath::Min(sync.timeBudgetMs + static_cast<int64>(elapsed), maxBudget); // 클라이언트가 이동할 수 있는 누적 시간 예산

	// ---------------------------------------------------------------
	// [3] 구간 검증
	// ---------------------------------------------------------------
	const bool	canMove = mc->CanMove(now);
	const float	speed = mc->GetMoveSpeed();

	const Vector3 startLocation = player->GetLocation();
	Vector3 location = startLocation;
	float yaw = player->GetYaw();
	uint64 processedTick = mc->GetLastMoveProcessedClientTick();

	bool settled = false;
	bool violated = false;

	Violation violation;

	for (size_t i = 1; i < jobs.size(); ++i)
	{
		const MoveJob& from = jobs[i - 1];
		const MoveJob& to = jobs[i];

		// 후딜 중: 클라 보고를 수용하지 않고 서버 위치를 유지한다. (위반으로 보지도 않는다)
		// 서버는 후딜 시작 시각을 클라 tick으로 알 수 없어서, 후딜 직전까지의 정상 이동 보고와 구분할 수 없기 때문이다.
		// 후딜 중에 움직인 클라는 후딜이 끝난 뒤 연속성 검사에서 걸러진다.
		if (canMove == false)
		{
			sync.frozen = true;
			continue;
		}

		// (1) 연속성: 기준 위치가 서버가 인정한 위치에서 크게 벗어나 있으면 거부 (순간이동 방어)
		//     후딜이 끝난 직후 첫 구간만 한도를 넓게 본다.
		const float continuityLimit = Config::Validation::MOVE_SNAP_DISTANCE * (sync.frozen ? Config::Validation::MOVE_FROZEN_LIMIT_MULT : 1.f);
		const float continutyLimitSq = continuityLimit * continuityLimit;
		if (from.location.Distance2DSq(location) > continutyLimitSq)
		{
			violated = true;
			violation = Violation{ L"continuity", i, from.location.Distance2D(location), continuityLimit, 0, location, from.location, false };
			break;
		}
		sync.frozen = false;

		// (2) 속도: 클라가 이 구간에서 스스로 이동했다고 주장하는 거리가 한도 이내인가
		uint64 dtMs = UtilMath::Min(to.sendClientTick - from.sendClientTick, Config::Validation::MOVE_MAX_JOB_DELTA_MS);
		float allowed = Config::Validation::MOVE_REACH_EPSILON;

		if (from.velocity.IsZero() == false)
		{
			dtMs = UtilMath::Min(dtMs, static_cast<uint64>(UtilMath::Max(budget, (int64)0)));
			budget -= static_cast<int64>(dtMs);
			allowed += speed * (static_cast<float>(dtMs) * 0.001f) * Config::Validation::MOVE_SPEED_TOLERANCE;
		}

		const float claimedSq = from.location.Distance2DSq(to.location);

		if (claimedSq > (allowed * allowed))
		{
			violated = true;
			violation = Violation{ L"speed", i, std::sqrt(claimedSq), allowed, dtMs, location, to.location, true};
			break;
		}

		// (3) 지형
		if (GGridMap->CanGo(to.location) == false)
		{
			violated = true;
			violation = Violation{ L"terrain", i, std::sqrt(claimedSq), allowed, dtMs, location, to.location, false };
			
			break;
		}

		// 수용
		location = to.location;
		yaw = to.yaw;
		processedTick = to.sendClientTick;
		settled = to.velocity.IsZero();
	}

	sync.timeBudgetMs = budget;

	// ---------------------------------------------------------------
	// [4] 결과 반영
	// ---------------------------------------------------------------
	mc->SetLastMoveProcessedClientTick(processedTick);

	const bool locationChanged = location.Distance2DSq(startLocation) > 0.1f; // 미세한 오차의 경우 이동으로 판단하지 않음
	const bool yawChanged = (yaw != player->GetYaw());

	if (locationChanged)
	{
		player->SetLocation(location);
		player->SetDirtyFlag(PLAYER_DIRTY_PLAYER_DETAIL);
	}

	if (yawChanged)
		player->SetYaw(yaw);

	if (violated)
	{
		LOG_WARN(L"[MoveViolation] " + std::wstring(violation.reason)
			+ L" seg: " + std::to_wstring(violation.segment)
			+ L" dt: " + std::to_wstring(violation.dtMs)
			+ L" claimed: " + std::to_wstring(violation.claimed) + L"/" + std::to_wstring(violation.allowed)
			+ L" speed: " + std::to_wstring(speed)
			+ L" server: (" + std::to_wstring(violation.serverLocation.x) + L", " + std::to_wstring(violation.serverLocation.y) + L")"
			+ L" client: (" + std::to_wstring(violation.clientLocation.x) + L", " + std::to_wstring(violation.clientLocation.y) + L")");

		if (violation.isSpeed && GServerStats)
			GServerStats->game.suspiciousPacket.fetch_add(1);

		// 보정은 한 번만 보내고, 클라가 보정 위치에서 보고할 때까지 옛 보고는 버린다.
		sync.awaitingResync = true;
		sync.resyncStartTick = now;
		NotifyCorrection(player);
	}

	result.changed = locationChanged || yawChanged;
	result.settled = settled;
	return result;
}
```
- 가장 메인이 되는 코드로 이동 로직의 중심을 담당하고있다.
- 로직 처리 순서는
  1. 상태 확인
  2. Job Load 후 시간 순 정렬
  3. 재동기화 대기
  4. 시간 예산 산정
  5. 구간 검증
     1) 기준위치 검증
     2) 속도 검증
     3) 지형검증
     4) 수용
  6. 결과 반영 순으로 진행된다.

재동기화는 현재 Player의 ResetBase() (이전 이동 시 위반으로 인한 갱신, 최소 접속, 부활 등에 호출) 시 resync가 true로 변경되며 이전 패킷 폐기
시간 예산은 clientTick의 조작으로 인한 속도핵 방지
구간검증에서
기준위치 검증은 클라이언트 패킷의 현재위치 값과 비교하여 오류 검증
속도 검증은 클라의 이동거리 검증
지형 검증은 이동 가능한 위치 검증
으로 유효성검사를 진행하게 된다.

## 개선된 코드의 한계와 개선 방향
- MAX_BUDGET의 크기가 현재 1000ms 로 적용되어 있고, 지형검사는 MOVEJob의 끝점에서만 진행하고 있기 때문에 얇은 벽을 통과하는 버그가 발생할 수 있음
  -> 지형검사의 CanGo에서 시작점과 끝점의 위치를 같이 넘겨서 중간 부분의 추가적인 충돌 테스트를 진행 할 필요가 있음
- 정상패킷의 속도 여유를 두기위해 사용하는 MOVE_SPEED_TOLERANCE(속도대비 상대오차)가 1.2일 경우, 1.2배로 속도를 조작해서 보내는 클라이언트는 정상 클라이언트로 동작할 수 있음
  -> 정상 클라이언트의 서버 & 클라이언트의 오차를 추적하여 MOVE_SPEED_TOLERANCE의 범위를 줄여나가는 방법을 사용할 수 있음
