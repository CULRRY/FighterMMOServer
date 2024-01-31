#include "stdafx.h"
#include "Server.h"

#include "Character.h"
#include "Direction.h"
#include "Log.h"
#include "NetworkManager.h"
#include "PacketHandler.h"
#include "SectorManager.h"
#include "SessionManager.h"

void Server::Init()
{
	wcout.imbue(locale("kor"));
	timeBeginPeriod(1);
	NetworkManager::Init();
	SectorManager::Init();
}

void Server::Run()
{
	Server::Init();
	_globalTime = timeGetTime();
	_frameTime = timeGetTime();
	_logicTime = timeGetTime();
	while (shutDown)
	{
		PROFILE_SAVE(VK_SHIFT);

		_totalDeath += _deathCount;

		if (timeGetTime() - _frameTime >= 1000)
		{
			LOG_SYSTEM(
				L"\n\
FPS        : {:<4}\n\
Sessions   : {:<4}\n\
Characters : {:<4}\n\
--------------------------\n\
{:<4} accept     /sec\n\
{:<4} disconnect /sec\n\
{:<4} death      /sec | total : {}\n\
{:<4} timeout    /sec\n\
{:<4} packet     /sec\n\
--------------------------\n\
Network Time : {:<4} ms\n\
Select  Time : {:<4} ms\n\
Accept  Time : {:<4} ms\n\
Recv    Time : {:<4} ms\n\
Send    Time : {:<4} ms\n\
Logic   Time : {:<4} ms\n\
--------------------------\n\
\n\n\n\n\n\n\
",
				_frame, 
				SessionManager::Count(), 
				_characterMap.size(), 
				_acceptCount, 
				SessionManager::disconnectCount,
				_deathCount,
				_totalDeath,
				NetworkManager::timeOutCount,
				NetworkManager::throughPut,
				networkTime,
				NetworkManager::selectTime,
				NetworkManager::acceptTime,
				NetworkManager::recvTime,
				NetworkManager::sendTime,
				_logicTime
			);

			_frame = 0;
			_frameTime += 1000;
			NetworkManager::throughPut = 0;
			NetworkManager::timeOutCount = 0;
			NetworkManager::selectTime = 0;
			NetworkManager::acceptTime = 0;
			NetworkManager::recvTime   = 0;
			NetworkManager::sendTime   = 0;
			_deathCount = 0;
			_acceptCount = 0;
			SessionManager::disconnectCount = 0;
			_logicTime = 0;
			networkTime = 0;
		}

		uint32 start = timeGetTime();
		NetworkManager::ProcessNetworkIO();
		networkTime += (timeGetTime() - start);

		
		Update();
		SessionManager::DisconnectAll();
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
	uint32 timeDelay = timeGetTime() - _globalTime;
	if (timeDelay < 40)
		return false;
	uint32 start = timeGetTime();

	_frame++;
	for (auto it : _characterMap)
	{
		Character* character = it.second;

		if (character->IsAlive() == false)
		{
			_deathCount++;
			SessionManager::ReserveDisconnect(character->GetSession());
		}

		character->Move();
	}

	_logicTime += timeGetTime() - start;
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
