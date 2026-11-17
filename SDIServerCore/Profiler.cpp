#include "pch.h"
#include "Profiler.h"
#include <sstream>
#include <chrono>
#include <iomanip>

void ProfileSample::Begin()
{
	flag = true;

	::QueryPerformanceCounter(&startTime);
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
		newProfile.processTimeMin.store(INT64_MAX);
		newProfile.processTimeMax.store(0);
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
			int64 timeMin = INT64_MAX;
			int64 timeMax = 0;
			int64 avg = 0;

			for (ProfileSample& profile : profiles)
			{
				totalTime += profile.totalTime.exchange(0);
				callTimes += profile.callTimes.exchange(0);

				int64 dTimeMin = profile.processTimeMin.exchange(INT64_MAX);
				int64 dTimeMax = profile.processTimeMax.exchange(0);

				if (dTimeMin < timeMin) timeMin = dTimeMin;
				if (dTimeMax > timeMax) timeMax = dTimeMax;
			}

			_sumProfile[tagName].totalTime.fetch_add(totalTime);
			_sumProfile[tagName].callTimes.fetch_add(callTimes);

			if (callTimes > 0)
			{
				avg = totalTime / callTimes;
				avg = (avg * 1000000 / _freq.QuadPart); // us 출력
				timeMin = (timeMin * 1000000 / _freq.QuadPart);
				timeMax = (timeMax * 1000000 / _freq.QuadPart);
			}
			else
			{
				timeMin = 0;
				avg = 0;
			}
			
			wstring tag = L"[" + tagName + L"]";

			stream << std::left << std::setw(25) << tag;
			stream << L"   call: "	<< std::right << std::setw(7)	<< callTimes	<< L"/s";
			stream << L"   avg: "	<< std::right << std::setw(5)	<< avg			<< L"us";
			stream << L"   min: "	<< std::right << std::setw(5)	<< timeMin		<< L"us";
			stream << L"   max: "	<< std::right << std::setw(7)	<< timeMax		<< L"us";
			stream << L" (Total: "	<< std::right << std::setw(10)	<< _sumProfile[tagName].callTimes.load() << L")";
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

Profile::Profile(wstring tagName) : _tagName(tagName)
{ 
	PROFILE_BEGIN(tagName); 
}
Profile::~Profile()
{
	PROFILE_END(_tagName);
}

