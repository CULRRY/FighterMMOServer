#include "stdafx.h"
#include "Direction.h"
#include "PacketHandler.h"

#include "Action.h"
#include "Character.h"
#include "Session.h"
#include "Packet.h"
#include "SectorManager.h"
#include "Server.h"
#include "SessionManager.h"

bool PacketHandler::HandlePacket(Session* session)
{
	if (session->GetRecvBuffer().GetUseSize() < sizeof(PacketHeader))
	{
		return false;
	}

	PacketHeader header;
	session->GetRecvBuffer().Peek(reinterpret_cast<BYTE*>(&header), sizeof(PacketHeader));

	if (header.code != 0x89)
	{
		SessionManager::ReserveDisconnect(session);
		return false;
	}

	if (session->GetRecvBuffer().GetUseSize() < header.size + sizeof(PacketHeader))
	{
		return false;
	}

	session->GetRecvBuffer().MoveFront(sizeof(PacketHeader));
	Packet pkt(header.size);
	session->GetRecvBuffer().Dequeue(pkt.GetBufferPtr(), header.size);
	pkt.MoveWritePos(header.size);

	uint64 now = timeGetTime();
	session->SetTime(now);

	switch (header.type)
	{
	case PacketType::C_MOVE_START:
	{
		Direction dir;
		int16 x;
		int16 y;

		pkt >> dir >> x >> y;
		Handle_C_MOVE_START(session, dir, x, y);
		break;
	}
	case PacketType::C_MOVE_STOP:
	{
		Direction dir;
		int16 x;
		int16 y;

		pkt >> dir >> x >> y;
		Handle_C_MOVE_STOP(session, dir, x, y);
		break;
	}
	case PacketType::C_ATTACK1:
	{
		Direction dir;
		int16 x;
		int16 y;

		pkt >> dir >> x >> y;
		Handle_C_ATTACK1(session, dir, x, y);
		break;
	}
	case PacketType::C_ATTACK2:
	{
		Direction dir;
		int16 x;
		int16 y;

		pkt >> dir >> x >> y;
		Handle_C_ATTACK2(session, dir, x, y);
		break;
	}
	case PacketType::C_ATTACK3:
	{
		Direction dir;
		int16 x;
		int16 y;

		pkt >> dir >> x >> y;
		Handle_C_ATTACK3(session, dir, x, y);
		break;
	}
	case PacketType::C_ECHO:
	{
		uint32 time;
		pkt >> time;
		Handle_C_ECHO(session, time);
	}

	default:
		return false;
		break;
	}

	return true;
}

bool PacketHandler::Handle_C_MOVE_START(Session* session, Direction& dir, int16 x, int16 y)
{
	Character* character = Server::FindCharacter(session);

	if (character == nullptr)
	{
		return false;
	}

	character->SetAction(Action::MOVE);
	character->GetMoveDir() = dir;

	switch (dir)
	{
	case Direction::LL:
	case Direction::LU:
	case Direction::LD:
		character->GetDir() = Direction::LL;
		break;

	case Direction::RR:
	case Direction::RU:
	case Direction::RD:
		character->GetDir() = Direction::RR;
		break;

	default:
		break;
	}

	if (character->IsAllowableRange(x, y) == false)
	{
		Packet syncPkt;
		PacketHandler::Make_S_SYNC(
			syncPkt, 
			character->GetSessionId(), 
			character->GetX(), 
			character->GetY()
		);
		SectorManager::SendAround(session, character->GetSector(), syncPkt);
	}
	else
	{
		character->SetPosition(y, x);
	}

	if (character->UpdateSector() == false)
	{
		Packet sendPkt;
		PacketHandler::Make_S_MOVE_START(sendPkt, session->GetId(), dir, x, y);
		SectorManager::SendAround(session, character->GetSector(), sendPkt);
	}

	//if (session->GetId() == 0)
	//{
	//	cout << "[START] " << x << " " << y << " " << (int32)dir << endl;
	//}

	return true;
}

bool PacketHandler::Handle_C_MOVE_STOP(Session* session, Direction& dir, int16 x, int16 y)
{
	Character* character = Server::FindCharacter(session);

	if (character == nullptr)
	{
		return false;
	}

	character->SetAction(Action::STOP);
	character->GetDir() = dir;

	if (character->IsAllowableRange(x, y) == false)
	{
		Packet syncPkt;
		PacketHandler::Make_S_SYNC(
			syncPkt,
			character->GetSessionId(),
			character->GetX(),
			character->GetY()
		);
		SectorManager::SendAround(session, character->GetSector(), syncPkt);
	}
	else
	{
		character->SetPosition(y, x);
	}

	character->UpdateSector();

	Packet sendPkt;
	PacketHandler::Make_S_MOVE_STOP(sendPkt, session->GetId(), dir, x, y);

	SectorManager::SendAround(session, character->GetSector(), sendPkt);

	//if (session->GetId() == 0)
	//{
	//	cout << "[STOP] " << x << " " << y << " " << (int32)dir << endl;
	//}


	return true;
}

bool PacketHandler::Handle_C_ATTACK1(Session* session, Direction& dir, int16 x, int16 y)
{
	Character* character = Server::FindCharacter(session);
	if (character == nullptr)
	{
		return false;
	}


	if (character->GetAction() == Action::STOP || character->GetAction() == Action::MOVE)
	{
		if (character->IsAllowableRange(x, y) == false)
		{
			Packet syncPkt;
			PacketHandler::Make_S_SYNC(
				syncPkt,
				character->GetSessionId(),
				character->GetX(),
				character->GetY()
			);
			SectorManager::SendAround(session, character->GetSector(), syncPkt);
			character->UpdateSector();
		}
		else
		{
			character->SetPosition(y, x);
		}

		character->UpdateSector();
		character->GetDir() = dir;
		


		Packet sendPkt;
		PacketHandler::Make_S_ATTACK1(sendPkt, character->GetSessionId(), dir, x, y);
		SectorManager::SendAround(character->GetSession(), character->GetSector(), sendPkt);

		Sector* sector = character->GetSector();

		for (auto it = sector->Begin(); it != sector->End(); ++it)
		{
			Character* other = it->second;
			if (character == other)
			{
				continue;
			}

			int8 damage = 5;

			if (Server::IsAttackRange(character, other, Server::ATTACK1_RANGE_X, Server::ATTACK1_RANGE_Y))
			{
				character->Attack(other, damage);

				Packet damagePkt;
				PacketHandler::Make_S_DAMAGE(damagePkt, character->GetSessionId(), other->GetSessionId(), other->GetHp());
				SectorManager::SendAround(nullptr, character->GetSector(), damagePkt);
			}
		}

	}

	return true;
}

bool PacketHandler::Handle_C_ATTACK2(Session* session, Direction& dir, int16 x, int16 y)
{
	Character* character = Server::FindCharacter(session);
	if (character == nullptr)
	{
		return false;
	}

	if (character->GetAction() == Action::STOP || character->GetAction() == Action::MOVE)
	{
		if (character->IsAllowableRange(x, y) == false)
		{
			Packet syncPkt;
			PacketHandler::Make_S_SYNC(
				syncPkt,
				character->GetSessionId(),
				character->GetX(),
				character->GetY()
			);
			SectorManager::SendAround(session, character->GetSector(), syncPkt);
			character->UpdateSector();
		}
		else
		{
			character->SetPosition(y, x);
		}

		character->UpdateSector();
		character->GetDir() = dir;



		Packet sendPkt;
		PacketHandler::Make_S_ATTACK2(sendPkt, character->GetSessionId(), dir, x, y);
		SectorManager::SendAround(character->GetSession(), character->GetSector(), sendPkt);

		Sector* sector = character->GetSector();

		for (auto it = sector->Begin(); it != sector->End(); ++it)
		{
			Character* other = it->second;
			if (character == other)
			{
				continue;
			}

			int8 damage = 5;

			if (Server::IsAttackRange(character, other, Server::ATTACK2_RANGE_X, Server::ATTACK2_RANGE_Y))
			{
				character->Attack(other, damage);

				Packet damagePkt;
				PacketHandler::Make_S_DAMAGE(damagePkt, character->GetSessionId(), other->GetSessionId(), other->GetHp());
				SectorManager::SendAround(nullptr, character->GetSector(), damagePkt);
			}
		}

	}
	return true;
}

bool PacketHandler::Handle_C_ATTACK3(Session* session, Direction& dir, int16 x, int16 y)
{
	Character* character = Server::FindCharacter(session);
	if (character == nullptr)
	{
		return false;
	}

	if (character->GetAction() == Action::STOP || character->GetAction() == Action::MOVE)
	{
		if (character->IsAllowableRange(x, y) == false)
		{
			Packet syncPkt;
			PacketHandler::Make_S_SYNC(
				syncPkt,
				character->GetSessionId(),
				character->GetX(),
				character->GetY()
			);
			SectorManager::SendAround(session, character->GetSector(), syncPkt);
			character->UpdateSector();
		}
		else
		{
			character->SetPosition(y, x);
		}

		character->UpdateSector();
		character->GetDir() = dir;



		Packet sendPkt;
		PacketHandler::Make_S_ATTACK3(sendPkt, character->GetSessionId(), dir, x, y);
		SectorManager::SendAround(character->GetSession(), character->GetSector(), sendPkt);

		Sector* sector = character->GetSector();

		for (auto it = sector->Begin(); it != sector->End(); ++it)
		{
			Character* other = it->second;
			if (character == other)
			{
				continue;
			}

			int8 damage = 5;

			if (Server::IsAttackRange(character, other, Server::ATTACK3_RANGE_X, Server::ATTACK3_RANGE_Y))
			{
				character->Attack(other, damage);

				Packet damagePkt;
				PacketHandler::Make_S_DAMAGE(damagePkt, character->GetSessionId(), other->GetSessionId(), other->GetHp());
				SectorManager::SendAround(nullptr, character->GetSector(), damagePkt);
			}
		}

	}
	return true;
}

bool PacketHandler::Handle_C_ECHO(Session* session, uint32 time)
{
	Packet pkt;
	Make_S_ECHO(pkt, time);
	SessionManager::SendUnicast(session, pkt);
	return true;
}

bool PacketHandler::Make_S_CREATE_MY_CHARACTER(Packet& pkt, int32 id, Direction& dir, int16 x, int16 y, uint8 hp)
{
	pkt << id << dir << x << y << hp;
	pkt.MakeHeader(PacketType::S_CREATE_MY_CHARACTER);
	return true;
}

bool PacketHandler::Make_S_CREATE_OTHER_CHARACTER(Packet& pkt, int32 id, Direction& dir, int16 x, int16 y, uint8 hp)
{
	pkt << id << dir << x << y << hp;
	pkt.MakeHeader(PacketType::S_CREATE_OTHER_CHARACTER);
	return true;
}

bool PacketHandler::Make_S_DELETE_CHARACTER(Packet& pkt, int32 id)
{
	pkt << id;
	pkt.MakeHeader(PacketType::S_DELETE_CHARACTER);
	return true;
}

bool PacketHandler::Make_S_MOVE_START(Packet& pkt, int32 id, Direction& dir, int16 x, int16 y)
{
	pkt << id << dir << x << y;
	pkt.MakeHeader(PacketType::S_MOVE_START);
	return true;
}

bool PacketHandler::Make_S_MOVE_STOP(Packet& pkt, int32 id, Direction& dir, int16 x, int16 y)
{
	pkt << id << dir << x << y;
	pkt.MakeHeader(PacketType::S_MOVE_STOP);
	return true;
}

bool PacketHandler::Make_S_ATTACK1(Packet& pkt, int32 id, Direction& dir, int16 x, int16 y)
{
	pkt << id << dir << x << y;
	pkt.MakeHeader(PacketType::S_ATTACK1);
	return true;
}

bool PacketHandler::Make_S_ATTACK2(Packet& pkt, int32 id, Direction& dir, int16 x, int16 y)
{
	pkt << id << dir << x << y;
	pkt.MakeHeader(PacketType::S_ATTACK2);
	return true;
}

bool PacketHandler::Make_S_ATTACK3(Packet& pkt, int32 id, Direction& dir, int16 x, int16 y)
{
	pkt << id << dir << x << y;
	pkt.MakeHeader(PacketType::S_ATTACK3);
	return true;
}

bool PacketHandler::Make_S_DAMAGE(Packet& pkt, int32 attackId, int32 damageId, uint8 damageHp)
{
	pkt << attackId << damageId << damageHp;
	pkt.MakeHeader(PacketType::S_DAMAGE);
	return true;
}

bool PacketHandler::Make_S_SYNC(Packet& pkt, int32 id, int16 x, int16 y)
{
	pkt << id << x << y;
	pkt.MakeHeader(PacketType::S_SYNC);
	return true;
}

bool PacketHandler::Make_S_ECHO(Packet& pkt, uint32 time)
{
	pkt << time;
	pkt.MakeHeader(PacketType::S_ECHO);
	return true;
}
