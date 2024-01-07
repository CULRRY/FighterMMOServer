#include "stdafx.h"
#include "Sector.h"

#include "Character.h"
#include "Packet.h"
#include "Session.h"
#include "SessionManager.h"

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
			SessionManager::Disconnect(session);
			continue;
		}

		session->GetSendBuffer().Enqueue(pkt.GetBufferPtr(), pkt.GetDataSize());
	}
}
