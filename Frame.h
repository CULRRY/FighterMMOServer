#pragma once
class Frame
{
public:
	inline static DWORD fps = 0;

	static void Init(int32 frame);
	static void Sleep();
private:
	inline static DWORD startTime = 0;
	inline static DWORD idealTime = 0;
	inline static DWORD overTime = 0;
	inline static DWORD fpsTime = 0;
	inline static DWORD fpsMs = 0;
	inline static DWORD frameCount = 0;
};

