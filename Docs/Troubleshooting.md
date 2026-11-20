# 트러블슈팅 사례


## 1. SendBuffer Overwrite
- 환경 : ServerTick 50 ms

### 문제 확인
- Dummy Client 를 이용해 테스트를 진행하는 중에 한 번에 대량의 Dummy가 접속할 경우 CRASH가 발생([SDIServerCore/SendBuffer.cpp](https://github.com/namgungkwon/GameServer/blob/main/SDIServerCore/SendBuffer.cpp))
<img width="601" height="404" alt="SendBuffer Overwrite Crash" src="https://github.com/user-attachments/assets/6be3d05c-4c62-48a0-b711-187c504fce5b" />

- AllocSize가 SendBufferChunkSize 보다 크게 배정되어 발생하는 오류로 확인

### 원인 파악
- 서버가 안정화 되고, 플레이어가 World에 골고루 퍼져있을 때는 문제가 발생하지 않았지만, 부하 테스트를 위해서 같은 Zone에 Dummy를 몰아 넣으니 바로 문제가 발생하는 것을 확인하였다.
- CallStack을 확인하여 많은 메모리를 필요로 하는 호출 부분이 어느부분인지 찾아낼 수 있었다.
<img width="608" height="256" alt="SendBuffer Overwrite Call Stack" src="https://github.com/user-attachments/assets/a0418f79-2e22-4a18-ad80-1ead5d33fb54" />

### 대처
- 해당 PendingJob() 에서 송신하는 패킷의 Byte의 최대 크기를 계산해보았다.
- Spawn Packet (382 * N) bytes, Despawn Packet (1 + (10 * N)) bytes, Move packet (39 * N) bytes([Common/Protocol.proto](https://github.com/namgungkwon/GameServer/blob/main/Common/Protobuf/Protocol.proto))
   - Google Proto는 같은 uint64 여도 크기에 따라 가변적인 크기로 보내기 때문에 최대 길이로 최대한 보수적으로 계산하였다. ex) 0 ~ 127 - 2byte, 128 ~ 16383 - 3bytes, ... , 최대 11 bytes
   - 각 패킷의 크기는 Perplexity 를 사용하여 최대 크기를 계산하였음.
- GameConfig에 해당 값을 기입하고 사용부에서 패킷이 일정 크기를 넘을 경우 바로 송신하여 일정 Byte를 넘지 못하게 코드를 개선하였다.

<img width="995" height="269" alt="SendBuffer Overwrite Config" src="https://github.com/user-attachments/assets/c21b2a4c-7919-4c1c-8d93-46cad62eab13" />

- GameConfig 개선 부([GameServer/GameConfig.h](https://github.com/namgungkwon/GameServer/blob/main/GameServer/GameConfig.h))

<img width="467" height="257" alt="SendBuffer Overwrite Code Update" src="https://github.com/user-attachments/assets/99e20d60-f529-468b-8792-1bb40b5cb36a" />

- ClearPendingJob 개선 부([GameServer/Zone.cpp](https://github.com/namgungkwon/GameServer/blob/main/GameServer/Zone.cpp))

- 매번 추가할 때마다 Packet의 크기를 계산하는 방법도 생각해보았지만, Protobuf의 크기 계산은 재귀적으로 멤버를 모두 순회하는 방식을 사용하여, 비용 문제가 발생할 것으로 판단하였다.(추후 테스트를 통해 비교 예정)

### 결과
- 목표 수치인 CCU 3000 을 기준으로 한 번에 3000개의 Dummy를 접속시켜도, 오류 없이 안정적으로 서버가 유지되었다.

## 2. CCU 3000 에서 서버 틱이 밀리는 현상
- 환경 : ServerTick 30 ms, 3000 CCU, 몬스터 등 추가부하 없음

### 문제확인
- CCU 3000, ServerTick 30ms 부하테스트 중 서버 틱이 최대 60ms 까지 밀리는 현상이 발생하였다.
<img width="1307" height="501" alt="PacketSender 적용 전" src="https://github.com/user-attachments/assets/21a704b0-cec0-4c71-9821-2c7874f562a9" />

- ServerStats([SDIGameServer/ServerStats](https://github.com/namgungkwon/GameServer/blob/main/SDIServerCore/ServerStats.cpp)) 과 Profiler([SDIGameServer/Profiler](https://github.com/namgungkwon/GameServer/blob/main/SDIServerCore/Profiler.cpp)) 로 문제 확인
  
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
  - 일감을 다른 스레드가 처리하기 때문에 패킷 순서를 보장하기 위해서, 하나의 GameScene에서는 정해진 Sender에만 일감을 넘길 수 있게 구현하였다.(Zone생성 시 GameScene에 따른 Sender배정 ([GameServer/World](https://github.com/namgungkwon/GameServer/blob/main/GameServer/World.cpp), CreateZone() 참고)
 
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

# 기타 발생 문제들([오류수정사항/txt](https://github.com/namgungkwon/GameServer/blob/main/%EC%98%A4%EB%A5%98%20%EC%88%98%EC%A0%95%20%EC%82%AC%ED%95%AD.txt))
| 문제 | 원인 | 결과 | 해결 후 배운 점 |
|---|---|---|---|
| 서버 공회전시에도 CPU 점유율 40% | CondVar의 Wake 조건이 잘못되었음 | 개별Thread점유율 8% -> 0 ~ 0.2% 로 정상적인 동작 확인 | CondVar 사용에 있어서 많은 지식을 얻을 수 있었다 |
| CREATE_ID 시에 같은 아이디 중복 생성 가능 | wchar[255] name에서 실제 아이디 길이를 제외한 부분이 쓰레기 값으로 들어가 있어서 byte를 읽는 DB에서 전혀 다른 값으로 받아들이게 된 것 | 같은 아이디 중복 처리 확인 | 문자열 변환에 있어서 지식의 부족함을 알았다 |
| Client 종료시 접속 유저의 이동 오류 | 코드를 수정하는 과정에서 return을 continue로 변경해주지 않아서 발생함 | 접속 종료시에도 정상적인 이동 수행확인 | 코드를 수정하고 난 이후에 해당 함수의 흐름을 전체 점검하는 습관을 들여야겠다는 생각을 하게 되었다.|
