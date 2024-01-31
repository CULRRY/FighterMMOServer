#include "stdafx.h"
#include "Packet.h"
#include "SectorManager.h"

#include "Direction.h"

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
