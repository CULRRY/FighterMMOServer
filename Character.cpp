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

	_x = 120;
	_y = 120;
	_action = Action::STOP;
	_dir = dir;
	_moveDir = dir;
	_sector = SectorManager::GetSector(_y / Sector::HEIGHT, _x / Sector::WIDTH);

	_sector->Insert(this);
}

void Character::UpdateSector()
{
	Sector* newSector = SectorManager::GetSector(_y / Sector::HEIGHT, _x / Sector::WIDTH);

	if (newSector == _sector)
		return;

	vector<Sector*> newAdjacent;
	vector<Sector*> newDelete;
	newAdjacent.reserve(5);
	newDelete.reserve(5);

	switch (_moveDir)
	{
		using enum Direction;
	case LL:
		newAdjacent.push_back(SectorManager::GetSector(newSector->_y, newSector->_x - 1));
		newAdjacent.push_back(SectorManager::GetSector(newSector->_y + 1, newSector->_x - 1));
		newAdjacent.push_back(SectorManager::GetSector(newSector->_y - 1, newSector->_x - 1));

		newDelete.push_back(SectorManager::GetSector(_sector->_y, _sector->_x + 1));
		newDelete.push_back(SectorManager::GetSector(_sector->_y + 1, _sector->_x + 1));
		newDelete.push_back(SectorManager::GetSector(_sector->_y - 1, _sector->_x + 1));
		break;
	case LU: 
		newAdjacent.push_back(SectorManager::GetSector(newSector->_y, newSector->_x - 1));
		newAdjacent.push_back(SectorManager::GetSector(newSector->_y + 1, newSector->_x - 1));
		newAdjacent.push_back(SectorManager::GetSector(newSector->_y - 1, newSector->_x - 1));
		newAdjacent.push_back(SectorManager::GetSector(newSector->_y - 1, newSector->_x));
		newAdjacent.push_back(SectorManager::GetSector(newSector->_y - 1, newSector->_x + 1));

		newDelete.push_back(SectorManager::GetSector(_sector->_y, _sector->_x + 1));
		newDelete.push_back(SectorManager::GetSector(_sector->_y + 1, _sector->_x + 1));
		newDelete.push_back(SectorManager::GetSector(_sector->_y - 1, _sector->_x + 1));
		newDelete.push_back(SectorManager::GetSector(_sector->_y + 1, _sector->_x));
		newDelete.push_back(SectorManager::GetSector(_sector->_y + 1, _sector->_x - 1));
		break;
	case UU: 
		newAdjacent.push_back(SectorManager::GetSector(newSector->_y - 1, newSector->_x));
		newAdjacent.push_back(SectorManager::GetSector(newSector->_y - 1, newSector->_x + 1));
		newAdjacent.push_back(SectorManager::GetSector(newSector->_y - 1, newSector->_x - 1));

		newDelete.push_back(SectorManager::GetSector(_sector->_y + 1, _sector->_x - 1));
		newDelete.push_back(SectorManager::GetSector(_sector->_y + 1, _sector->_x + 1));
		newDelete.push_back(SectorManager::GetSector(_sector->_y + 1, _sector->_x));
		break;
	case RU: 
		newAdjacent.push_back(SectorManager::GetSector(newSector->_y, newSector->_x + 1));
		newAdjacent.push_back(SectorManager::GetSector(newSector->_y + 1, newSector->_x + 1));
		newAdjacent.push_back(SectorManager::GetSector(newSector->_y - 1, newSector->_x + 1));
		newAdjacent.push_back(SectorManager::GetSector(newSector->_y - 1, newSector->_x));
		newAdjacent.push_back(SectorManager::GetSector(newSector->_y - 1, newSector->_x - 1));

		newDelete.push_back(SectorManager::GetSector(_sector->_y, _sector->_x - 1));
		newDelete.push_back(SectorManager::GetSector(_sector->_y + 1, _sector->_x - 1));
		newDelete.push_back(SectorManager::GetSector(_sector->_y - 1, _sector->_x - 1));
		newDelete.push_back(SectorManager::GetSector(_sector->_y + 1, _sector->_x));
		newDelete.push_back(SectorManager::GetSector(_sector->_y + 1, _sector->_x + 1));
		break;
	case RR: 
		newAdjacent.push_back(SectorManager::GetSector(newSector->_y, newSector->_x + 1));
		newAdjacent.push_back(SectorManager::GetSector(newSector->_y + 1, newSector->_x + 1));
		newAdjacent.push_back(SectorManager::GetSector(newSector->_y - 1, newSector->_x + 1));

		newDelete.push_back(SectorManager::GetSector(_sector->_y, _sector->_x - 1));
		newDelete.push_back(SectorManager::GetSector(_sector->_y + 1, _sector->_x - 1));
		newDelete.push_back(SectorManager::GetSector(_sector->_y - 1, _sector->_x - 1));
		break;
	case RD: 
		newAdjacent.push_back(SectorManager::GetSector(newSector->_y, newSector->_x + 1));
		newAdjacent.push_back(SectorManager::GetSector(newSector->_y + 1, newSector->_x + 1));
		newAdjacent.push_back(SectorManager::GetSector(newSector->_y - 1, newSector->_x + 1));
		newAdjacent.push_back(SectorManager::GetSector(newSector->_y + 1, newSector->_x));
		newAdjacent.push_back(SectorManager::GetSector(newSector->_y + 1, newSector->_x - 1));

		newDelete.push_back(SectorManager::GetSector(_sector->_y, _sector->_x - 1));
		newDelete.push_back(SectorManager::GetSector(_sector->_y + 1, _sector->_x - 1));
		newDelete.push_back(SectorManager::GetSector(_sector->_y - 1, _sector->_x - 1));
		newDelete.push_back(SectorManager::GetSector(_sector->_y - 1, _sector->_x));
		newDelete.push_back(SectorManager::GetSector(_sector->_y - 1, _sector->_x + 1));
		break;
	case DD: 
		newAdjacent.push_back(SectorManager::GetSector(newSector->_y + 1, newSector->_x));
		newAdjacent.push_back(SectorManager::GetSector(newSector->_y + 1, newSector->_x + 1));
		newAdjacent.push_back(SectorManager::GetSector(newSector->_y + 1, newSector->_x - 1));

		newDelete.push_back(SectorManager::GetSector(_sector->_y - 1, _sector->_x - 1));
		newDelete.push_back(SectorManager::GetSector(_sector->_y - 1, _sector->_x + 1));
		newDelete.push_back(SectorManager::GetSector(_sector->_y - 1, _sector->_x));
		break;
	case LD: 
		newAdjacent.push_back(SectorManager::GetSector(newSector->_y, newSector->_x - 1));
		newAdjacent.push_back(SectorManager::GetSector(newSector->_y + 1, newSector->_x - 1));
		newAdjacent.push_back(SectorManager::GetSector(newSector->_y - 1, newSector->_x - 1));
		newAdjacent.push_back(SectorManager::GetSector(newSector->_y + 1, newSector->_x));
		newAdjacent.push_back(SectorManager::GetSector(newSector->_y + 1, newSector->_x + 1));

		newDelete.push_back(SectorManager::GetSector(_sector->_y, _sector->_x + 1));
		newDelete.push_back(SectorManager::GetSector(_sector->_y + 1, _sector->_x + 1));
		newDelete.push_back(SectorManager::GetSector(_sector->_y - 1, _sector->_x + 1));
		newDelete.push_back(SectorManager::GetSector(_sector->_y - 1, _sector->_x));
		newDelete.push_back(SectorManager::GetSector(_sector->_y - 1, _sector->_x - 1));
		break;
	default: ;
	}

	_sector->Delete(this);
	newSector->Insert(this);

	_sector = newSector;

	for (Sector* sector : newAdjacent)
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

	for (Sector* sector : newDelete)
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
}

void Character::Move()
{
	static int16 dx[] = { -6, -6, 0, 6, 6, 6, 0, -6 };
	static int16 dy[] = { 0, -4, -4, -4, 0, 4, 4, 4 };

	if (_action == Action::STOP)
		return;

	int16 nx = _x + dx[static_cast<int8>(_moveDir)];
	int16 ny = _y + dy[static_cast<int8>(_moveDir)];

	if (nx < Server::RANGE_MOVE_LEFT || nx > Server::RANGE_MOVE_RIGHT
		|| ny < Server::RANGE_MOVE_TOP || ny > Server::RANGE_MOVE_BOTTOM)
		return;

	_x = nx;
	_y = ny;

	UpdateSector();
	//cout << session->x << " " << session->y << endl;
}

