#include "stdafx.h"
#include "Character.h"

#include "Action.h"
#include "Session.h"
#include "Direction.h"
#include "PacketHandler.h"
#include "SectorManager.h"
#include "Server.h"
#include "SessionManager.h"

Character::Character(Session* session)
	:_session(session), _sessionId(session->GetId()), _hp(100)
{
	int16 randVal = rand();
	int16 randX = randVal % Server::RANGE_MOVE_RIGHT;

	randVal = rand();
	int16 randY = randVal % Server::RANGE_MOVE_BOTTOM;

	if (randX < Server::RANGE_MOVE_LEFT)
		randX += Server::RANGE_MOVE_LEFT;
	if (randY < Server::RANGE_MOVE_TOP)
		randY += Server::RANGE_MOVE_TOP;
	Direction dir;
	if (randX < Server::RANGE_MOVE_RIGHT / 2)
	{
		dir = Direction::RR;
	}
	else
	{
		dir = Direction::LL;
	}

	_x = randX;
	_y = randY;
	_action = Action::STOP;
	_dir = dir;
	_moveDir = dir;
	_sector = SectorManager::GetSector(_y / Sector::HEIGHT, _x / Sector::WIDTH);

	_sector->Insert(this);
}

bool Character::UpdateSector()
{
	if (_y >= 6400)
	{
		_y = 6399;
	}

	if (_x >= 6400)
	{
		_x = 6399;
	}

	Sector* newSector = SectorManager::GetSector(_y / Sector::HEIGHT, _x / Sector::WIDTH);

	if (newSector == _sector)
		return false;


	PROFILE_BEGIN(L"UpdateSector()");
	vector<Sector*>* newAdjacent = nullptr;
	vector<Sector*>* newDelete = nullptr;

	int16 dy = newSector->GetY() - _sector->GetY();
	int16 dx = newSector->GetX() - _sector->GetX();


	Direction changeDir;

	switch (dy)
	{
		using enum Direction;
	case -1:
		switch (dx)
		{
		case -1:
			changeDir = LU;
			newAdjacent = &newSector->LeftTop();
			newDelete = &_sector->RightBottom();
			break;
		case 0:
			changeDir = UU;
			newAdjacent = &newSector->Top();
			newDelete = &_sector->Bottom();
			break;
		case 1:
			changeDir = RU;
			newAdjacent = &newSector->RightTop();
			newDelete = &_sector->LeftBottom();
			break;
		}
		break;
	case 0:
		switch (dx)
		{
		case -1:
			changeDir = LL;
			newAdjacent = &newSector->Left();
			newDelete = &_sector->Right();
			break;
		case 1:
			changeDir = RR;
			newAdjacent = &newSector->Right();
			newDelete = &_sector->Left();
			break;
		}
		break;
	case 1:
		switch (dx)
		{
		case -1:
			changeDir = LD;
			newAdjacent = &newSector->LeftBottom();
			newDelete = &_sector->RightTop();
			break;
		case 0:
			changeDir = DD;
			newAdjacent = &newSector->Bottom();
			newDelete = &_sector->Top();
			break;
		case 1:
			changeDir = RD;
			newAdjacent = &newSector->RightBottom();
			newDelete = &_sector->LeftTop();
			break;
		}
	}

	_sector->Delete(this);
	newSector->Insert(this);

	_sector = newSector;

	for (Sector* sector : *newAdjacent)
	{
		if (sector == nullptr)
			continue;

		Packet pkt;
		PacketHandler::Make_S_CREATE_OTHER_CHARACTER(pkt, _sessionId, _moveDir, _x, _y, _hp);
		sector->Broadcast(nullptr, pkt);
		if (_action == Action::MOVE)
		{
			Packet movePkt;
			PacketHandler::Make_S_MOVE_START(
				movePkt,
				_sessionId,
				_moveDir,
				_x,
				_y
			);

			sector->Broadcast(_session, movePkt);
		}


		for (auto session : sector->_sessionList)
		{
			Character* character = session.second;
			pkt.Clear();
			PacketHandler::Make_S_CREATE_OTHER_CHARACTER(
				pkt,
				character->_sessionId,
				character->_moveDir,
				character->_x,
				character->_y,
				character->_hp
			);

			SessionManager::SendUnicast(_session, pkt);

			if (character->_action == Action::MOVE)
			{
				Packet movePkt;
				PacketHandler::Make_S_MOVE_START(
					movePkt,
					character->GetSessionId(),
					character->_moveDir,
					character->_x,
					character->_y
				);

				SessionManager::SendUnicast(_session, movePkt);
			}

			
		}
	}

	for (Sector* sector : *newDelete)
	{
		if (sector == nullptr)
			continue;

		Packet pkt;
		PacketHandler::Make_S_DELETE_CHARACTER(pkt, _sessionId);
		sector->Broadcast(nullptr, pkt);

		for (auto session : sector->_sessionList)
		{
			Character* character = session.second;
			pkt.Clear();
			PacketHandler::Make_S_DELETE_CHARACTER(
				pkt,
				character->_sessionId
			);

			SessionManager::SendUnicast(_session, pkt);
		}
	}

	PROFILE_END(L"UpdateSector()");

	return true;
}

void Character::Move()
{

	if (_action == Action::STOP)
		return;

	int16 nx = _x + dx[static_cast<int8>(_moveDir)] * 6;
	int16 ny = _y + dy[static_cast<int8>(_moveDir)] * 4;

	if (nx < Server::RANGE_MOVE_LEFT || nx > Server::RANGE_MOVE_RIGHT
		|| ny < Server::RANGE_MOVE_TOP || ny > Server::RANGE_MOVE_BOTTOM)
		return;

	_x = nx;
	_y = ny;

	UpdateSector();

	//if (_sessionId == 0)
	//{
	//	cout << _x << " " << _y << " " << (int32)_moveDir << endl;
	//}
	//cout << session->x << " " << session->y << endl;
}

