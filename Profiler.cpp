#include "stdafx.h"
#include "Profiler.h"
#include "DateTime.h"
#include <fstream>
#include <format>

void Profiler::Begin(const wstring_view name)
{
	Sample* sample = findSample(name);

	if (sample == nullptr)
	{
		sample = getEmptySample();
		sample->name = name;
		sample->callCount = 0;
	}
	else
	{
		if (sample->startTime.QuadPart != NULLTIME)
		{
			__debugbreak();
		}

		//sample->callCount++;
	}

	::QueryPerformanceCounter(&sample->startTime);
}

void Profiler::End(wstring_view name, LARGE_INTEGER endTime)
{
	Sample* sample = findSample(name);

	if (sample == nullptr)
	{
		__debugbreak();
	}

	if (sample->startTime.QuadPart == NULLTIME)
	{
		__debugbreak();
	}

	const uint64 processTime = endTime.QuadPart - sample->startTime.QuadPart;

	sample->callCount += 1;
	sample->totalTime += processTime;
	sample->startTime.QuadPart = NULLTIME;

	// 최대, 최소값 갱신
	for (uint64& i : sample->max)
	{
		if (processTime > i)
		{
			i = processTime;
			break;
		}
	}

	for (uint64& i : sample->min)
	{
		if (processTime < i)
		{
			i = processTime;
			break;
		}

		if (i == 0)
		{
			i = processTime;
			break;
		}
	}
}

void Profiler::DataOut(wstring_view fileName)
{
	wofstream fout(fileName.data());
	fout.imbue(std::locale("kor"));
	fout << L"┌----------------------------------┬-------------------┬-------------------┬-------------------┬---------------┐\n";
	fout << L"│               Name               │      Average      │        Min        │        Max        │     Call      │\n";
	fout << L"├----------------------------------┼-------------------┼-------------------┼-------------------┼---------------┤\n";

	const int64 freq = getPerformanceFrequency().QuadPart;
	const double perMicroSecond = static_cast<double>(freq) / static_cast<double>(SECOND_PER_MICRO_SECOND);

	for (const Sample& sample : profileSamples)
	{
		double timeAverage;
		if (sample.callCount == 0)
		{
			continue;
		}

		if (sample.callCount > 4)
		{
			timeAverage =
				static_cast<double>(sample.totalTime - (sample.max[0] + sample.max[1] + sample.min[0] + sample.min[1]))
				/ (sample.callCount - 4) / perMicroSecond;
		}
		else
		{
			timeAverage =
				static_cast<double>(sample.totalTime) / sample.callCount / perMicroSecond;
		}

		fout << ::format(
			L"│ {:<32} │ {:>14.1f} μs │ {:>14.1f} μs │ {:>14.1f} μs │ {:>13} │\n",
			sample.name,
			timeAverage,
			sample.min[0] / perMicroSecond,
			sample.max[0] / perMicroSecond,
			sample.callCount
			);
	}

	fout << L"└----------------------------------┴-------------------┴-------------------┴-------------------┴---------------┘\n";

	DateTime localTime;

	fout << ::format(
		L"Updated at {}\n",
		localTime.to_string()
	);

	fout.close();
}

void Profiler::DataOut()
{
	DateTime localTime;

	wstring fileName = ::format(L"profile/Profile_{}.txt",
		localTime.to_string(L"%Y%m%d_%H%M%S")
	);

	DataOut(fileName);
}

void Profiler::CSVOut(wstring_view fileName)
{
	wofstream fout(fileName.data());
	fout.imbue(std::locale("kor"));

	fout << "name,average,min,max,call\n";


	const int64 freq = getPerformanceFrequency().QuadPart;
	const double perMicroSecond = static_cast<double>(freq) / static_cast<double>(SECOND_PER_MICRO_SECOND);

	for (const Sample& sample : profileSamples)
	{
		double timeAverage;
		if (sample.callCount == 0)
		{
			continue;
		}

		if (sample.callCount > 4)
		{
			timeAverage =
				static_cast<double>(sample.totalTime - (sample.max[0] + sample.max[1] + sample.min[0] + sample.min[1]))
				/ (sample.callCount - 4) / perMicroSecond;
		}
		else
		{
			timeAverage =
				static_cast<double>(sample.totalTime) / sample.callCount / perMicroSecond;
		}

		fout << ::format(
			L"{},{},{},{},{}\n",
			sample.name,
			timeAverage,
			sample.min[0] / perMicroSecond,
			sample.max[0] / perMicroSecond,
			sample.callCount
		);
	}
}

void Profiler::CSVOut()
{
	DateTime localTime;

	wstring fileName = ::format(L"profile/Profile_{}.csv",
		localTime.to_string(L"%Y%m%d_%H%M%S")
	);

	CSVOut(fileName);
}

void Profiler::Reset()
{
	::memset(profileSamples, 0, sizeof(profileSamples));
}

LARGE_INTEGER Profiler::getPerformanceFrequency()
{
	static LARGE_INTEGER frequency{ 0, 0 };

	if (frequency.QuadPart == 0)
	{
		::QueryPerformanceFrequency(&frequency);
	}

	return frequency;
}

Profiler::Sample* Profiler::findSample(wstring_view name)
{
	int idx = 0;

	while (profileSamples[idx].useFlag != false)
	{
		if (profileSamples[idx].name.compare(name) == 0)
		{
			return &profileSamples[idx];
		}

		idx++;
	}

	return nullptr;
}

Profiler::Sample* Profiler::getEmptySample()
{
	Sample* space = profileSamples;
	while (space->useFlag != false)
	{
		space++;
	}

	space->useFlag = true;

	return space;
}
