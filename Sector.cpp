#include "stdafx.h"
#include "Sector.h"

#include "Character.h"
#include "Log.h"
#include "Packet.h"
#include "Session.h"
#include "SessionManager.h"
#include "Direction.h"
#include "SectorManager.h"

Sector::Sector(int16 y, int16 x) : _y(y), _x(x)
{
	for (int32 d = 0; d < 8; d++)
	{
		int16 ny = y + dy[d];
		int16 nx = x + dx[d];

		if (ny < 0 || ny >= 6400 / HEIGHT || nx < 0 || nx >= 6400 / WIDTH)
		{
			continue;
		}

		Sector* sector = SectorManager::GetSector(ny, nx);

		_sectorsAround.push_back(sector);

		Direction changeDir;

		switch (dy[d])
		{
			using enum Direction;
		case -1:
			switch (dx[d])
			{
			case -1:
				changeDir = LU;
				_sectorsLeft.push_back(sector);
				_sectorsTop.push_back(sector);
				_sectorsLeftTop.push_back(sector);
				break;
			case 0:
				changeDir = UU;
				_sectorsRightTop.push_back(sector);
				_sectorsTop.push_back(sector);
				_sectorsLeftTop.push_back(sector);
				break;
			case 1:
				changeDir = RU;
				_sectorsRight.push_back(sector);
				_sectorsTop.push_back(sector);
				_sectorsRightTop.push_back(sector);
				break;
			}
			break;
		case 0:
			switch (dx[d])
			{
			case -1:
				changeDir = LL;
				_sectorsLeft.push_back(sector);
				_sectorsLeftTop.push_back(sector);
				_sectorsLeftBottom.push_back(sector);
				break;
			case 1:
				changeDir = RR;
				_sectorsRight.push_back(sector);
				_sectorsRightTop.push_back(sector);
				_sectorsRightBottom.push_back(sector);
				break;
			}
			break;
		case 1:
			switch (dx[d])
			{
			case -1:
				changeDir = LD;
				_sectorsLeft.push_back(sector);
				_sectorsLeftBottom.push_back(sector);
				_sectorsBottom.push_back(sector);
				break;
			case 0:
				changeDir = DD;
				_sectorsRightBottom.push_back(sector);
				_sectorsBottom.push_back(sector);
				_sectorsLeftBottom.push_back(sector);
				break;
			case 1:
				changeDir = RD;
				_sectorsRight.push_back(sector);
				_sectorsRightBottom.push_back(sector);
				_sectorsBottom.push_back(sector);
				break;
			}
		}

	}
}

void Sector::Insert(Character* session)
{
	_sessionList.insert({ session->GetSessionId(), session });
}

void Sector::Delete(Character* session)
{
	_sessionList.erase(session->GetSessionId());
}

void Sector::Move(Character* session, Sector* sector)
{
	_sessionList.erase(session->GetSessionId());
	sector->_sessionList.insert({ session->GetSessionId(), session });
}

void Sector::Broadcast(Session* except, Packet& pkt)
{
	for (auto it = _sessionList.begin(); it != _sessionList.end(); ++it)
	{
		Session* session = it->second->GetSession();

		if (except == session)
			continue;

		if (session->GetSendBuffer().GetFreeSize() < pkt.GetDataSize())
		{
			LOG_ERROR(L"Full Send Buffer");
			SessionManager::ReserveDisconnect(session);
			continue;
		}

		session->GetSendBuffer().Enqueue(pkt.GetBufferPtr(), pkt.GetDataSize());
	}
}
