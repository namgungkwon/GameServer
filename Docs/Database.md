# DB 설계

## Tables

### 1. Account

| Field | Type | Cond | Desc |
|---|---|---|---|
| accountId | bigint |  | 계정별 고유 Id |
| id | nvarchar(255) | primaryKey | 아이디 |
| password | nvarchar(100) |  | 추후 Hash 변경 예정 |

### 2. Players
   
| Field | Type | Cond | Desc |
|---|---|---|---|
| accountId | bigint |  | 계정별 고유 Id |
| playerId | bigint | primaryKey | 플레이어 고유 Id |
| templateId | int |  | 직업 |
| name | nvarchar(100) |  | 캐릭터이름 |
| level | int | default(1) | 레벨 |
- index : I1(accountId, playerId)

### 3. PlayerDetail
| Field | Type | Cond | Desc |
|---|---|---|---|
| playerId | bigint | primaryKey | 플레이어고유 Id |
| pos_x | float |  | X좌표 |
| pos_y | float |  | Y좌표 |
| pos_z | float |  | Z좌표 |
| yaw | float |  | 회전값 |
| hp | int |  | 체력 |
| mp | int |  | 마나 |
| exp | bigint |  | 경험치 |


## DBManager ([GameServer/DBmanager.h](https://github.com/namgungkwon/GameServer/blob/main/GameServer/DBManager.h))

- struct DBConn -> 일반적인 DB Query에 사용하는 구조체
- struct DBTransactionGuard -> 트랜잭션 시작·커밋·롤백을 보장하기 위한 RAII 기반 트랜잭션 관리 구조체

### 요청별 처리

| 요청 | DB처리 |
|---|---|
| CG_CREATE_ACCOUNT | DBTransaction(account 열 추가) |
| CG_LOGIN | account 조회 |
| CG_CREATE_CHARACTER | DBTransaction(Players 열 추가 -> Players에서 PlayerId조회 -> PlayerDetail 추가 -> PlayerList조회) |
| CG_ENTER_GAME | playerDetail 조회 |
