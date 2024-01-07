#include "stdafx.h"
#include "SessionManager.h"

#include "Packet.h"
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

void SessionManager::Disconnect(Session* session)
{
}

void SessionManager::SendUnicast(Session* session, Packet& pkt)
{
	if (session->GetSendBuffer().GetFreeSize() < pkt.GetDataSize())
	{
		Disconnect(session);
		return;
	}

	session->GetSendBuffer().Enqueue(pkt.GetBufferPtr(), pkt.GetDataSize());
}
