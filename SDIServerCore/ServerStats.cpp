#include "pch.h"
#include "ServerStats.h"
#include <chrono>
#include <Psapi.h>
#include <sstream>

#pragma comment(lib, "psapi.lib")


///////////////////////////////////////////////////////
//	<	ServerStats	>
///////////////////////////////////////////////////////
void SystemStats::Refresh()
{
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)))   // 프로세스의 메모리 사용량을 가져온다.
        memUsageMB = pmc.WorkingSetSize / (1 * Config::MB);

    static ULARGE_INTEGER prevKernel{}, prevUser{}, prevWall{};
    FILETIME ftNow, ftCreation, ftExit, ftKernel, ftUser;
    GetSystemTimeAsFileTime(&ftNow);                                                        // ftNow - 현재시간
    GetProcessTimes(GetCurrentProcess(), &ftCreation, &ftExit, &ftKernel, &ftUser);

    ULARGE_INTEGER curKernel, curUser, curNow;
    curKernel.LowPart = ftKernel.dwLowDateTime; curKernel.HighPart = ftKernel.dwHighDateTime; // curKernel - 커널모드에서 사용한 시간
    curUser.LowPart = ftUser.dwLowDateTime; curUser.HighPart = ftUser.dwHighDateTime; // curUser - 유저모드에서 사용한 시간
    curNow.LowPart = ftNow.dwLowDateTime; curNow.HighPart = ftNow.dwHighDateTime; // curNow - 현재 시간(wall-clock time)

    if (prevWall.QuadPart != 0)
    {
        uint64 cpuTime = (curKernel.QuadPart - prevKernel.QuadPart) + (curUser.QuadPart - prevUser.QuadPart); // cpuTime 계산
        uint64 wallTime = curNow.QuadPart - prevWall.QuadPart;             // deltaTime 계산

        SYSTEM_INFO si;
        GetSystemInfo(&si);
        cpuUsage = (100.0 * cpuTime) / wallTime / si.dwNumberOfProcessors; // cpu 사용 시간을 논리 프로세스 갯수로 나누어 cpu 사용량 계산
    }

    prevKernel = curKernel; 
    prevUser = curUser; 
    prevWall = curNow;
}

void ServerStats::Init()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD  mode = 0;
    if (GetConsoleMode(hOut, &mode))
        SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    _running = true;
}

void ServerStats::RunUpdateLoop()
{
    //GLogger->ReserveStatsPanel(STATS_PANEL_LINES);

    while (_running)
    {
        Rendering();
        std::this_thread::sleep_for(1s);
    }
}

void ServerStats::Rendering()
{
    system.Refresh();

    // Network
    int32 connectedSession  = network.connectedSessions.load();
    uint64 totalConnections = network.totalConnections.load();
    uint64 recvKB           = network.recvBytes.exchange(0) / (1 * Config::KB);
    uint64 sendKB           = network.sendBytes.exchange(0) / (1 * Config::KB);
    uint64 recvPackets      = network.recvPackets.exchange(0);
    uint64 sendPackets      = network.sendPackets.exchange(0);

    // Job
    uint64 jobsExecute      = job.jobsExecuted.exchange(0);

    // Iocp
    uint64 iocpCallCount    = iocp.iocpCallCount.exchange(0);
    uint64 iocpTime         = iocp.totalProcessTImeUs.exchange(0);
    uint64 avgIocpUs        = (iocpCallCount > 0) ? (iocpTime / iocpCallCount) : 0;

    // Memory
    uint64 poolHits         = memory.poolHitsCount.exchange(0);
    uint64 poolMiss         = memory.poolMissCount.exchange(0);
    uint64 liveAllocCount   = memory.liveAllocCount.load();

    // DB
    uint64 DBCount          = DB.queryCount.exchange(0);
    uint64 DBFail           = DB.queryFailed.load();
    uint64 DBTotalUs        = DB.queryTotalUs.exchange(0);

    // System
    double cpuUsage         = system.cpuUsage;
    size_t memUsageMB       = system.memUsageMB;

    // Game
    int32 activePlayers     = game.activePlayers.load();;
    uint64 tickCount        = game.tickCount.exchange(0);
    uint64 lagTickCount     = game.lagTickCount.exchange(0);
    uint64 lagTickMs        = game.lagTickMs.exchange(0);
    uint64 lagAvgMs         = (lagTickCount > 0) ? (lagTickMs / lagTickCount) : 0;
    uint64 lagTickMaxMs     = game.lagTickMaxMs.exchange(0);


    // \033[K       - 줄 끝까지 지우기 - 잔상 제거
    // \033[31m     - 텍스트를 빨간색으로 설정
    // \033[0m      - 텍스트 서식 초기화

    std::wstringstream stream;

    stream << L"============================= [ SERVER STATUS ] ===========================";
    stream << L"\033[K\n";

    // Network
    stream << L"[SESSION]";
    stream << L"   sessions: " << connectedSession << L" (Total : " << totalConnections << L")";
    stream << L"\033[K\n";
    stream << L"[NETWORK]";
    stream << L"   Recv: " << recvKB << L"KB/s (" << recvPackets << L"pkt/s)";
    stream << L"   Send: " << sendKB << L"KB/s (" << sendPackets << L"pkt/s)";
    stream << L"\033[K\n";

    // Job
    stream << L"[JOB]    ";
    stream << L"   Job: " << jobsExecute << L"/s";
    stream << L"\033[K\n";

    // Iocp
    stream << L"[IOCP]   ";
    stream << L"   Call: " << iocpCallCount << L"/s (" << avgIocpUs << L"us)";
    stream << L"\033[K\n";

    // Memory
    stream << L"[MEMORY] ";
    stream << L"   Hits: " << poolHits;
    stream << L"   Miss: " << poolMiss;
    stream << L"   Live: " << liveAllocCount;
    stream << L"\033[K\n";

    // DB
    stream << L"[DB]     ";
    stream << L"   Queries: " << DBCount << L"/s";
    stream << L"   Failed: " << DBFail;
    stream << L"   DBTime: " << DBTotalUs << L"us";
    stream << L"\033[K\n";

    // System
    stream << L"[CPU]    ";
    stream << L"   " <<  cpuUsage << L"%";
    stream << L"\033[K\n";
    stream << L"[MEM]    ";
    stream << L"   " << memUsageMB << L"MB";
    stream << L"\033[K\n";

    // Game
    stream << L"============================= [ Game STATUS ] ===========================";
    stream << L"\033[K\n";

    stream << L"[TICK]   ";
    stream << L"   Scene: " << tickCount << L"/s";
    stream << L"   AvgLag: " << lagAvgMs << L"ms";
    stream << L"   MaxLag: " << lagTickMaxMs << L"ms";
    stream << L"\033[K\n";

    stream << L"[WORLD]  ";
    stream << L" Players: " << activePlayers;
    stream << L"\033[K\n";

    stream << L"============================================================================";
    stream << L"\033[K\n";


    {
        LockGuard guard(GLogger->GetConsoleLock());

        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hOut, &csbi);
        COORD prevPos = csbi.dwCursorPosition;

        SetConsoleCursorPosition(hOut, { 0, ServerStatsStartLine });
        std::wcout << stream.str();
        std::wcout.flush();

        SetConsoleCursorPosition(hOut, prevPos);
    }
}


