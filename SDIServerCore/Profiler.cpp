#include "pch.h"
#include "Profiler.h"
#include <sstream>
#include <chrono>

#pragma optimize("", off)
void ProfileSample::Begin()
{
	flag = true;

	::QueryPerformanceCounter(&startTime);
	
	processTimeMin.store(-1);
	processTimeMax.store(0);
}


void ProfileSample::End()
{
	LARGE_INTEGER lEndTime;
	QueryPerformanceCounter(&lEndTime);
	int64 spendTime = lEndTime.QuadPart - startTime.QuadPart;

	if (spendTime < processTimeMin.load())
	{
		processTimeMin = spendTime;
	}
	else if (spendTime > processTimeMax.load())
	{
		processTimeMax = spendTime;
	}

	totalTime.fetch_add(spendTime);
	callTimes.fetch_add(1);

	flag = false;
}


void Profiler::Init(wstring fileName)
{
	_outFileName = fileName;
	_running = true;

	::QueryPerformanceFrequency(&_freq);
}

void Profiler::ProfileBegin(wstring tagName)
{
	for (auto& item : _profileList[tagName])
	{
		if (item.threadId != LThreadId)
			continue;

		item.Begin();
		return;
	}

	{
		WRITE_LOCK;
		ProfileSample& newProfile = _profileList[tagName].emplace_back();
		newProfile.threadId = LThreadId;
		newProfile.Begin();
	}
}

void Profiler::ProfileEnd(wstring tagName)
{
	for (auto& item : _profileList[tagName])
	{
		if (item.threadId != LThreadId)
			continue;

		if (item.flag == false)
			return;

		item.End();
	}
}


void Profiler::UpdateProfiler()
{
	while (_running)
	{
		ProfileRecord();
		std::this_thread::sleep_for(1s);
	}
}

void Profiler::ProfileRecord()
{
	// \033[K       - 줄 끝까지 지우기 - 잔상 제거
	// \033[31m     - 텍스트를 빨간색으로 설정
	// \033[0m      - 텍스트 서식 초기화

	std::wstringstream stream;

	stream << L"============================= [ PROFILER ] ===========================";
	stream << L"\033[K\n";

	{
		WRITE_LOCK;
		for (auto& [tagName, profiles] : _profileList)
		{
			int64 totalTime = 0;
			int64 callTimes = 0;

			for (ProfileSample& profile : profiles)
			{
				totalTime += profile.totalTime.exchange(0);
				callTimes += profile.callTimes.exchange(0);
			}

			_sumProfile[tagName].totalTime.fetch_add(totalTime);
			_sumProfile[tagName].callTimes.fetch_add(callTimes);

			int64 avg = (callTimes > 0) ? totalTime / callTimes : 0;
			avg = (avg * 1000000 / _freq.QuadPart);

			stream << L"[" << tagName << L"]";
			stream << L"   call: " << callTimes << L"/s (Total: " << _sumProfile[tagName].callTimes.load() << L")";
			stream << L"   avg: " << avg << L"us";
			stream << L"\033[K\n";
		}
	}
	
	stream << L"======================================================================";
	stream << L"\033[K\n";

	{
		LockGuard guard(GLogger->GetConsoleLock());

		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(hOut, &csbi);
		COORD prevPos = csbi.dwCursorPosition;

		SetConsoleCursorPosition(hOut, { 0, ProfilerStartLine });
		std::wcout << stream.str();
		std::wcout.flush();

		SetConsoleCursorPosition(hOut, prevPos);
	}
}

#pragma optimize("", on)