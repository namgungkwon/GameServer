# C++ IOCP 멀티플레이 게임 서버
> Window IOCP 비동기 네트워크를 이용하여 CCU 3000 에서 안정화를 목표로 구현하고 있습니다.



## 환경

1. 기술 스택
| 구분 | 기술 |
| --- | --- |
| Server | C++17, Windows IOCP, Google Protobuf, SQL Server, Redis(hiredis) |
| Client | Unreal(C++) |
| 패턴 | IOCP, JobQueue, MemoryPooling, Zone AOI, DB Write-Back(구현중) |
| 부하 | DummyClient(C++, VS, 300(조절가능) 당 Thread 1개 배정) |

2. 환경
