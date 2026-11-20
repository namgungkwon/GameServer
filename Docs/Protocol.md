# Protocol 설계

## 로그인 패킷 (LoginServer Common/Protobuf/Protocol.proto)

1. PacketList
* CL -> 클라이언트 요청
* LC -> 로그인서버 응답



|패킷|용도|내용|
|-|-|-|
|CL\_CREATE\_ACCOUNT|계정생성|id, password|
|LC\_CRAETE\_ACCOUNT|계정생성 응답|result|
|CL\_LOGIN|로그인|id, password|
|LC\_LOGIN|로그인 응답|result, token|



## 게임 패킷 (GameServer Common/Protobuf/Protocol.proto)

1. PacketList
* CG -> 클라이언트 요청
* GC -> 서버 응답

|패킷|용도|내용|
|-|-|-|
|CG\_AUTH\_TOKEN|토큰 검증|token|
|GC\_AUTH\_TOKEN|토큰 검증(Redis) 응답, 캐릭터 목록(DB)|token, repeated summary|
|CG\_CREATE\_CHARACTER|캐릭터 생성|name, templateId|
|GC\_CREATE\_CHARACTER|캐릭터 생성 응답(DB중복 확인), 업데이트 캐릭터 목록|result, repeated summary|
|CG\_ENTER\_GAME|인게임 입장|playerId|
|GC\_ENTER\_GAME|인게임 입장 응답|success, objectData|
|CG\_LEAVE\_GAME|퇴장|objectId|
|GC\_SPAWN|스폰|repeated objectId|
|GC\_DESPAWN|디스폰|repeated objectId|
|CG\_REVIVE|부활|objectId, option(현재위치, 스폰위치)|
|GC\_REVIVE|부활응답|posData, statData|
|CG\_MOVE|플레이어 이동|clientTick, moveData|
|GC\_MOVE|플레이어 이동 응답|repeated moveData|
|CG\_ATTACK|플레이어 공격|posData, repeated target, skillId, clientTick|
|GC\_ATTACK|플레이어 공격 응답|posData, skillId|
|GC\_DAMAGE|데미지처리|objectId, damage, isDead|
|GC\_REWARD|보상, 경험치|exp|
|GC\_LEVELUP|레벨업|objectData|
|CG\_CHAT|채팅|chat, type(world, local)|
|GC\_CHAT|채팅응답|name, chat|

* summary, pos, move, object 데이터에는 각각 objectId가 포함 되어있음

### 처리 흐름

* 이동 : PacketHandler - Player::SetPendingJob() >> SceneUpdate시 MovementSystem에서 유효성 검사 - SceneUpdate시 GC\_MOVE, zone 변경시 GC\_SPAWN, GC\_DESPAWN >> PacketSender에서 송신
* 공격 : PacketHandler - AttackRequest로 Job요청 >> SceneUpdate시 Execute호출 - 유효성검사 - GC\_Attack - 피격 판정 - GC\_Damage - 사망시 - GC\_REWARD >> PacketSender에서 송신
- 채팅 : PacketHandler - ChatRequest로 Job요청 >> SceneUpdate시 Execute호출 - 유효성검사 - GC\_CHAT(world or local) >> PacketSender에서 송신

