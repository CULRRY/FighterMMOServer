#include "stdafx.h"
#include "Frame.h"

void Frame::Init(int32 frame)
{
	fpsMs = frame;
	startTime = timeGetTime();
	idealTime = startTime;
	fpsTime = startTime;
}

void Frame::Sleep()
{
	frameCount++;
	if (timeGetTime() - fpsTime >= 1000)
	{
		fps = frameCount;
		fpsTime += 1000;
		frameCount = 0;
	}

	::Sleep(fpsMs - overTime);
	idealTime += fpsMs;
	overTime = timeGetTime() - idealTime;
	if (overTime >= fpsMs)
	{
		overTime = fpsMs;
	}
}
