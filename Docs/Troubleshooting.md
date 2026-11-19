# 트러블슈팅 사례

## 1. CCU 3000 에서 서버 틱이 밀리는 현상
- 환경 : ServerTick 30 ms, 3000 CCU, 몬스터 등 추가부하 없음

### 문제확인
- CCU 3000, ServerTick 30ms 부하테스트 중 서버 틱이 최대 60ms 까지 밀리는 현상이 발생하였다.
<img width="1307" height="501" alt="PacketSender 적용 전" src="https://github.com/user-attachments/assets/21a704b0-cec0-4c71-9821-2c7874f562a9" />

- ServerStats([SDIGameServer/ServerStats](https://github.com/namgungkwon/GameServer/blob/main/SDIServerCore/ServerStats.cpp)) 과 Profiler([SDIGameServer/Profiler](https://github.com/namgungkwon/GameServer/blob/main/SDIServerCore/Profiler.cpp) 로 문제 확인
  
### 원인 파악
- 현재 ServerTick에 가장 많은 자원을 쓰는 부분을 확인하기 위해서 Profiler를 이용하여 원인을 파악했다.
  -> 측정구간은 전체 구간인 GameSceneUpdate()와 그 세부 구간 ZoneUpdate(), ProcessMovingEntity(), ClearPendingJob() 을 프로파일링 하였다.

| 더미수 | 지연 | GameSceneUpdate() | ZoneUpdate() | ProcessMovingEntity() | ClearPendingJob()|
|---|---|---|---|---|---|
| 500  | 0 ms | 0.0 / 0.4 / 1.4 ms | 0.0 / 0.1 / 0.2 ms | 0.0 / 0.0 / 0.2 ms | 0.0 / 0.3 / 1.2 ms |
| 1000 | 0 ms | 0.1 / 1.2 / 4.6 ms | 0.1 / 0.1 / 0.4 ms | 0.0 / 0.0 / 0.5 ms | 0.0 / 1.0 / 4.4 ms |
| 1500 | 0 ms | 0.1 / 2.5 / 7.1 ms | 0.1 / 0.2 / 0.7 ms | 0.0 / 0.1 / 0.8 ms | 0.0 / 2.2 / 6.5 ms |
| 2000 | 0 ms | 0.2 / 4.5 / 11.3 ms | 0.1 / 0.3 / 0.9 ms | 0.0 / 0.1 / 1.1 ms | 0.0 / 4.1 / 10.6 ms |
| 2500 | 0 ~ 4 ms | 0.4 / 6.9 / 20.5 ms | 0.1 / 0.3 / 0.8 ms | 0.0 / 0.2 / 2.3 ms | 0.1 / 6.3 / 20.8 ms |
| 3000 | 0 ~ 48 ms | 1.2 / 9.9 / 30.0 ms | 0.1 / 0.4 / 2.2 ms | 0.0 / 0.3 / 2.6 ms | 0.7 / 9.2 / 26.7 ms |

- 측정수치는  GameSceneUpdate() (ZoneUpdate(), ProcessMovingEntity(), ClearPendingJob()) 표기
- 시간단위는 소수 첫째자리까지 표기(둘째자리에서 반올림)
- 각 측정별 단위시간 내에 최대 시간을 기록하기 때문에 개별 합산값이 전체구간 합산을 초과할 수 있음

- 더미수 별로 프로파일링을 해보니 대부분의 시간을 ClearPendingJob()에서 처리하는 것을 알 수 있었다.
  ClearPendingJob()에서는 매 틱마다 쌓인 패킷을 한 번에 보내주는 기능을 하는데 이 때 NetworkSend 부분까지 담당하기 때문에 부하가 많이 걸릴 것으로 확정하였다.

### 대처
  - NetworkSend부분은 InGameTick이 작동하는데에 관여를 하지 않는 결과를 송신하는 부분이기 때문에 별도의 스레드로 분리하기로 하였다.
  - 이에 PacketSender 클래스를 새로 만들어 패킷을 전달만 해주는 구조를 만들기로 하였다.
  - 일감을 다른 스레드가 처리하기 때문에 패킷 순서를 보장하기 위해서, 하나의 GameScene에서는 정해진 Sender에만 일감을 넘길 수 있게 구현하였다.
 
| 더미수 | 지연 | GameSceneUpdate() | ZoneUpdate() | ProcessMovingEntity() | ClearPendingJob()|
|---|---|---|---|---|---|
| 500  | 0 ms | 0.0 / 0.1 / 0.3 ms | 0.0 / 0.1 / 0.3 ms | 0.0 / 0.0 / 0.1 ms | 0.0 / 0.0 / 0.1 ms |
| 1000 | 0 ms | 0.0 / 0.2 / 0.8 ms | 0.1 / 0.1 / 0.6 ms | 0.0 / 0.0 / 0.1 ms | 0.0 / 0.0 / 0.3 ms |
| 1500 | 0 ms | 0.1 / 0.3 / 1.2 ms | 0.1 / 0.2 / 0.6 ms | 0.0 / 0.1 / 0.2 ms | 0.0 / 0.0 / 0.7 ms |
| 2000 | 0 ms | 0.1 / 0.4 / 2.3 ms | 0.1 / 0.3 / 1.7 ms | 0.0 / 0.0 / 0.5 ms | 0.0 / 0.1 / 2.0 ms |
| 2500 | 0 ms | 0.1 / 0.5 / 3.8 ms | 0.1 / 0.3 / 1.7 ms | 0.0 / 0.1 / 0.4 ms | 0.0 / 0.1 / 3.3 ms |
| 3000 | 0 ms | 0.2 / 0.7 / 5.2 ms | 0.1 / 0.4 / 3.5 ms | 0.0 / 0.1 / 3.0 ms | 0.0 / 0.2 / 3.7 ms |

### 결과
- 기존 값과 비교한 결과이다.
  
| 더미수 | PacketSender 사용 전 | PacketSender 사용 후 | 변화 |
|---|---|---|---|
| 500  | 0.4 ms | 0.1 ms | - 0.3 ms |
| 1000 | 1.2 ms | 0.2 ms | - 1.0 ms |
| 1500 | 2.5 ms | 0.3 ms | - 2.2 ms |
| 2000 | 4.5 ms | 0.4 ms | - 4.1 ms |
| 2500 | 6.9 ms | 0.5 ms | - 6.4 ms |
| 3000 | 9.9 ms | 0.7 ms | - 9.2 ms |
- 평균 값 비교를 했을 때 Tick 처리 시간이 획기적으로 줄어드는 결과를 얻을 수 있었다.
- 최대 값으로 비교를 하면 CCU 3000 기준 80 % 이상의 시간이 줄어들었다.
