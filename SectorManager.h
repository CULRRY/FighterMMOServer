#pragma once
#include "Sector.h"

class Packet;

class SectorManager
{
public:
	static void Init();
	static Sector* GetSector(int32 y, int32 x)
	{
		if (y < 0 || y >= 6400 / Sector::HEIGHT || x < 0 || x >= 6400 / Sector::WIDTH)
			return nullptr;
		return &_sectorArray[y][x];
	};

	static void SendSector(Sector* sector, Packet& pkt);
	static void SendAround(Session* except, Sector* sector, Packet& pkt);

private:
	inline static Sector _sectorArray[6400 / Sector::HEIGHT][6400 / Sector::WIDTH];
};

