#pragma once

#define PROFILE
#ifdef PROFILE
#define PROFILE_BEGIN(TagName)				Profiler::Begin(TagName)
#define PROFILE_END(TagName)				do										\
											{										\
												LARGE_INTEGER endTime;				\
												QueryPerformanceCounter(&endTime);	\
												Profiler::End(TagName, endTime);	\
											}										\
											while(false)

#define PROFILE_DATA_OUT_NAME(FileName)		Profiler::DataOut(FileName)
#define PROFILE_DATA_OUT()					Profiler::DataOut()
#define PROFILE_CSV_OUT_NAME(FileName)		Profiler::CSVOut(FileName)
#define PROFILE_CSV_OUT()					Profiler::CSVOut()
#define PROFILE_RESET()						Profiler::Reset()
#define PROFILE_SAVE(key)					do											\
											{											\
												if (GetAsyncKeyState(key))				\
												{										\
													PROFILE_DATA_OUT();					\
													wcout << L"PROFILE SAVED\n";		\
												}										\
											} while (false);
											
#else
#define PROFILE_BEGIN(TagName)
#define PROFILE_END(TagName)
#define PROFILE_DATA_OUT_NAME(FileName)
#define PROFILE_DATA_OUT()
#define PROFILE_CSV_OUT_NAME(FileName)
#define PROFILE_CSV_OUT()
#define PROFILE_RESET()
#define PROFILE_SAVE(key)
#endif



class Profiler
{
	enum : int64
	{
		NULLTIME = INT64_MAX,
		SECOND_PER_MICRO_SECOND = 1'000'000,
	};

	struct Sample
	{
		bool			useFlag{};
		wstring_view	name;
		LARGE_INTEGER	startTime{};
		uint64			totalTime{};
		uint64			min[2]{};
		uint64			max[2]{};
		uint64			callCount{};
	};

public:
	static void Begin(wstring_view name);
	static void End(wstring_view name, LARGE_INTEGER endTime);
	static void DataOut(wstring_view fileName);
	static void DataOut();
	static void CSVOut(wstring_view fileName);
	static void CSVOut();
	static void Reset();

private:
	static LARGE_INTEGER	getPerformanceFrequency();
	static Sample*			findSample(wstring_view name);
	static Sample*			getEmptySample();

private:
	inline static Sample profileSamples[100];  
};

