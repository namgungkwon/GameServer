#pragma once

#define PROFILE

#ifdef PROFILE
#define PROFILE_BEGIN(TagName)	if(GProfiler) GProfiler->ProfileBegin(TagName);
#define PROFILE_END(TagName)	if(GProfiler) GProfiler->ProfileEnd(TagName);
#else
#define PROFILE_BEGIN(TagName)	
#define PROFILE_END(TagName)
#endif //PROFILE


struct ProfileSample
{
public:
	ProfileSample() = default;
	ProfileSample(const ProfileSample& other) : totalTime(other.totalTime.load()), callTimes(other.callTimes.load())
		, processTimeMax(other.processTimeMax.load()), processTimeMin(other.processTimeMin.load()), flag(other.flag), threadId(other.threadId) {};
public:
	void Begin();
	void End();

public:
	int32 flag;
	uint64 threadId;

	LARGE_INTEGER startTime;
	Atomic<int64> totalTime = 0;
	Atomic<int64> callTimes = 0;

	Atomic<int64> processTimeMin = 0;
	Atomic<int64> processTimeMax = 0;
};

class Profiler
{
public:
	void Init(wstring fileName);
public:
	void ProfileBegin(wstring tagName);
	void ProfileEnd(wstring tagName);
	void UpdateProfiler();

private:
	void ProfileRecord();

private:
	LARGE_INTEGER _freq;

private:
	USE_LOCK;
	atomic<bool> _running =	false;
	wstring _outFileName;
	HashMap<wstring, Vector<ProfileSample>> _profileList; // tagName, Profile(스레드별)
	HashMap<wstring, ProfileSample>			_sumProfile;  // 누적 값 보존용
};