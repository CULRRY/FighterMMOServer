#include "stdafx.h"
#include "Packet.h"
#include "SectorManager.h"

void SectorManager::Init()
{
	for (int y = 0; y < 6400 / Sector::HEIGHT; y++)
	{
		for (int x = 0; x < 6400 / Sector::WIDTH; x++)
		{
			_sectorArray[y][x] = Sector(y, x);
		}
	}
}

void SectorManager::SendSector(Sector* sector, Packet& pkt)
{
	sector->Broadcast(nullptr, pkt);
}

void SectorManager::SendAround(Session* except, Sector* sector, Packet& pkt)
{
	static int32 dx[] = { 1, 1, 1, 0, 0, -1, -1, -1 };
	static int32 dy[] = { 1, -1, 0, 1, -1, 1, -1, 0 };

	sector->Broadcast(except, pkt);

	for (int32 d = 0; d < 8; d++)
	{
		int32 ny = sector->GetY() + dy[d];
		int32 nx = sector->GetX() + dx[d];

		if (ny < 0 || ny >= 6400 / Sector::HEIGHT || nx < 0 || nx >= 6400 / Sector::WIDTH)
		{
			continue;
		}
		_sectorArray[ny][nx].Broadcast(nullptr, pkt);
	}
}
