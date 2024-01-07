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
	while (shutDown)
	{
		NetworkManager::ProcessNetworkIO();

		Update();
	}

}

bool Server::OnAccept(Session* session)
{
	Character* newCharacter = new Character(session);

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

	for (auto it = SessionManager::Begin(); it != SessionManager::End(); ++it)
	{
		Session* session = *it;

		if (newCharacter->GetSession() == session)
		{
			continue;
		}

		pkt.Clear();

		Character* character = _characterMap.find(session->GetId())->second;

		PacketHandler::Make_S_CREATE_OTHER_CHARACTER(
			pkt,
			character->GetSessionId(),
			character->GetDir(),
			character->GetX(),
			character->GetY(),
			character->GetHp()
		);
		SessionManager::SendUnicast(newCharacter->GetSession(), pkt);
	}

	return true;

}

bool Server::Update()
{
	if (timeGetTime() - _globalTime < 40)
		return false;

	for (auto it : _characterMap)
	{
		Character* character = it.second;

		if (character->IsAlive())
		{
			SessionManager::Disconnect(character->GetSession());
		}

		character->Move();
	}

	_globalTime += 40;
	return true;
}
