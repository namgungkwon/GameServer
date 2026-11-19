# C++ IOCP 멀티플레이 게임 서버
> Window IOCP 비동기 네트워크를 이용하여 CCU 3000 에서 안정화를 목표로 구현하고 있습니다.

[패킷설계](https://github.com/namgungkwon/GameServer/tree/main/Docs/Protocol.md) |

## 환경

1. 기술 스택

| 구분 | 기술 |
|---|---|
| Server | C++17, Windows IOCP, Google Protobuf, SQL Server, Redis(hiredis) |
| Client | Unreal(C++) |
| 패턴 | IOCP, JobQueue, MemoryPooling, Zone AOI, DB Write-Back(구현중) |
| 부하 | DummyClient(C++, VS, 300(조절가능) 당 Thread 1개 배정) |

2. 환경

| 항목 | 값 |
|---|---|
| CPU | AMD Ryzen 5 5600X 6-Core Processor(Hyper Threading 12 Core, 4.2GHz) |
| RAM | 63.9 G(16 X 4, 3200 MT/s) |
| OS | Windows 11 |
| ENV | 단일 PC |
| BUILD | Release x64 |

3. 스레드 구성(임시, 추후 테스트 후 배정)

| 솔루션 | 구성|
|---|---|
| SDIServerCore | JobTimer(1, 고정), Logger(1, 고정) |
| LoginServer | NetworkThread(5), LogicThread(5), Profiler(1, 고정) |
| GameServer | NetworkThread(10), LogicThread(16), PacketSender(8, SceneCount), ServerStat(1, 고정), Profiler(1, 고정) |

4. 서버 구조
![서버 구조](https://github.com/namgungkwon/GameServer/tree/main/Docs/Server.png)

## 기능
1. 요약
- 비동기 I/O 기반 다중 클라이언트 처리
- 스레드 풀 기반 요청 처리
- 설정 파일 기반 포트·루트 디렉터리 구성([GameServer/server.json](https://github.com/namgungkwon/GameServer/blob/main/GameServer/server.json))
- 접속/요청 로깅 및 에러 처리

2. 사용 기술

| 기술 | 설명 |
|---|---|
| IOCP | Network WorkerThread 기반 Listener(Accept), Session(Recv/Send)([SDIServerCore/IocpCore.h](http://github.com/namgungkwon/GameServer/blob/main/SDIServerCore/IocpCore.h)) |
| JobQueue | GameWorld를 Scene 분할하여 각 Scene 별로 독립적인 실행을 보장([SDIServerCore/JobQueue.h](https://github.com/namgungkwon/GameServer/blob/main/SDIServerCore/JobQueue.h)) |
| SceneRequest | Scene내부 요청을 SceneRequest로 확장하여 GameScene의 코드와 처리를 분할([SDIServerCore/SceneRequest.h](https://github.com/namgungkwon/GameServer/blob/main/GameServer/SceneRequest.h)) |
| Zone AOI | 인접 Zone의 정보만 클라이언트와 공유하여 네트워크 부하관리([GameServer/Zone.cpp](https://github.com/namgungkwon/GameServer/blob/main/GameServer/Zone.cpp)) |
| Protobuf | Proto 파일에 작성된 정보를 읽어([Common/Protobuf](https://github.com/namgungkwon/GameServer/tree/main/Common/Protobuf)), GenPacket.exe로 코드 자동 생성(jinja2, [Tools/PacketGenerator.py](https://github.com/namgungkwon/GameServer/blob/main/Tools/PacketGenerator/PacketGenerator.py)) |
| SendBuffer 풀링 | Move, Spawn, Despawn등 일부 패킷을 Zone별로 SceneTick 기반 풀링하여 네트워크 부하를 줄임([GameServer/Zone.cpp](https://github.com/namgungkwon/GameServer/blob/main/GameServer/GameScene.cpp)) |
| PacketSender | GameScene의 SceceTick 과 NetworkSend를 분리하여 인게임 딜레이 감소를 추구([GameServer/PacketSender.cpp](https://github.com/namgungkwon/GameServer/blob/main/GameServer/PacketSender.cpp)) |
| GridMap | Unreal에서 생성한 레벨을 단위좌표기준으로 장애물을 검사하여 생성한 맵파일을 공유하여 사용([GameServer/Gridmap.cpp](https://github.com/namgungkwon/GameServer/blob/main/GameServer/GridMap.cpp)) |
| AI | GridMap을 기반으로 경로 탐색(A*, 추후 JPS 로 변경할 예정), FSM([GameServer/Npc.h](https://github.com/namgungkwon/GameServer/blob/main/GameServer/NPC.h)) |

### 구현 중
- DB Write-Back : 매번 DB를 갱신하지 않고, 일부 요청을 제외(아이템 관련)하고 DirtyFlag를 두어 일정 시간마다 비동기 저장

## 빌드 & 실행
- 추가 예정