#include "stdafx.h"
#include "Server.h"

#include "Character.h"
#include "Direction.h"
#include "NetworkManager.h"
#include "PacketHandler.h"
#include "SectorManager.h"
#include "SessionManager.h"

void Server::Init()
{
	NetworkManager::Init();
	SectorManager::Init();
	
}

void Server::Run()
{
	_globalTime = timeGetTime();
	_frameTime = timeGetTime();
	while (shutDown)
	{
		PROFILE_SAVE(VK_SHIFT);

		if (timeGetTime() - _frameTime >= 1000)
		{
			cout << _frame << endl;
			cout << SessionManager::Count() << endl;
			cout << _characterMap.size() << endl;
			cout << NetworkManager::throughPut << endl;

			_frame = 0;
			_frameTime += 1000;
		}


		PROFILE_BEGIN(L"IO");
		NetworkManager::ProcessNetworkIO();
		PROFILE_END(L"IO");

		PROFILE_BEGIN(L"Logic");
		Update();
		PROFILE_END(L"Logic");
	}

}

bool Server::OnAccept(Session* session)
{
	Character* newCharacter = _characterPool.Alloc(session);

	_characterMap.insert({ session->GetId(), newCharacter });

	Packet pkt;
	PacketHandler::Make_S_CREATE_MY_CHARACTER(
		pkt,
		newCharacter->GetSessionId(),
		newCharacter->GetDir(),
		newCharacter->GetX(),
		newCharacter->GetY(),
		newCharacter->GetHp()
	);

	SessionManager::SendUnicast(session, pkt);

	pkt.Clear();

	PacketHandler::Make_S_CREATE_OTHER_CHARACTER(
		pkt,
		newCharacter->GetSessionId(),
		newCharacter->GetDir(),
		newCharacter->GetX(),
		newCharacter->GetY(),
		newCharacter->GetHp()
	);

	SectorManager::SendAround(newCharacter->GetSession(), newCharacter->GetSector(), pkt);

	Sector* sector = newCharacter->GetSector();

	for (auto it = sector->Begin(); it != sector->End(); ++it)
	{
		Character* other = it->second;
		if (newCharacter == other)
		{
			continue;
		}

		pkt.Clear();

		PacketHandler::Make_S_CREATE_OTHER_CHARACTER(
			pkt,
			other->GetSessionId(),
			other->GetDir(),
			other->GetX(),
			other->GetY(),
			other->GetHp()
		);
		SessionManager::SendUnicast(newCharacter->GetSession(), pkt);
	}

	for (int32 d = 0; d < 8; d++)
	{
		int32 ny = sector->GetY() + dy[d];
		int32 nx = sector->GetX() + dx[d];

		if (ny < 0 || ny >= 6400 / Sector::HEIGHT || nx < 0 || nx >= 6400 / Sector::WIDTH)
		{
			continue;
		}
		Sector* aroundSector = SectorManager::GetSector(ny, nx);

		for (auto it = aroundSector->Begin(); it != aroundSector->End(); ++it)
		{
			Character* other = it->second;
			if (newCharacter == other)
			{
				continue;
			}

			pkt.Clear();

			PacketHandler::Make_S_CREATE_OTHER_CHARACTER(
				pkt,
				other->GetSessionId(),
				other->GetDir(),
				other->GetX(),
				other->GetY(),
				other->GetHp()
			);
			SessionManager::SendUnicast(newCharacter->GetSession(), pkt);
		}
	}

	return true;

}

bool Server::Update()
{
	if (timeGetTime() - _globalTime < 40)
		return false;

	_frame++;
	for (auto it : _characterMap)
	{
		Character* character = it.second;

		if (character->IsAlive() == false)
		{
			SessionManager::ReserveDisconnect(character->GetSession());
		}

		character->Move();


	}

	_globalTime += 40;
	return true;
}


void Server::DeleteCharacter(Session* session)
{
	auto item =_characterMap.find(session->GetId());
	_characterPool.Free(item->second);

	_characterMap.erase(session->GetId());
}

bool Server::IsAttackRange(Character* character, Character* target, int32 rangeX, int32 rangeY)
{
	if (target->GetY() < character->GetY() - rangeY || target->GetY() >= character->GetY() + rangeY)
	{
		return false;
	}

	if (character->GetDir() == Direction::LL)
	{
		if (target->GetX() > character->GetX() || target->GetX() <= character->GetX() - rangeX)
		{
			return false;
		}
	}
	else
	{
		if (target->GetX() < character->GetX() || target->GetX() >= character->GetX() + rangeX)
		{
			return false;
		}
	}

	return true;
}
