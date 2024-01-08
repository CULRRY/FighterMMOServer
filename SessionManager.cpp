#include "stdafx.h"
#include "SessionManager.h"

#include "Character.h"
#include "Packet.h"
#include "PacketHandler.h"
#include "Sector.h"
#include "SectorManager.h"
#include "Server.h"
#include "SocketUtil.h"

void SessionManager::Init()
{
}

Session* SessionManager::Create(SOCKET sock, SOCKADDR_IN sockAddr)
{
	Session* newSession = sessionPool.Alloc(sock, sockAddr);

	_sessionList.push_back(newSession);

	return newSession;
}

void SessionManager::ReserveDisconnect(Session* session)
{
	Character* character = Server::FindCharacter(session);
	if (character != nullptr)
	{
		character->GetSector()->Delete(character);
		Server::DeleteCharacter(session);

		Packet pkt;
		PacketHandler::Make_S_DELETE_CHARACTER(pkt, session->GetId());
		SectorManager::SendAround(session, character->GetSector(), pkt);
	}

	session->OnDisconnect();
}

void SessionManager::SendUnicast(Session* session, Packet& pkt)
{
	if (session->GetSendBuffer().GetFreeSize() < pkt.GetDataSize())
	{
		ReserveDisconnect(session);
		return;
	}

	session->GetSendBuffer().Enqueue(pkt.GetBufferPtr(), pkt.GetDataSize());
}

void SessionManager::DisconnectAll()
{
	auto it = _sessionList.begin();

	while (it != _sessionList.end())
	{
		Session* session = *it;
		if (session->IsDisconnect())
		{
			sessionPool.Free(session);
			it = _sessionList.erase(it);
		}
		else
		{
			++it;
		}
	}
}
