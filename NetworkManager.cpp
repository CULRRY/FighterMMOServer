#include "stdafx.h"
#include "NetworkManager.h"

#include "Server.h"
#include "Session.h"
#include "SessionManager.h"
#include "SocketUtil.h"

void NetworkManager::Init()
{
	SocketUtil::Init();
	srand(10);

	_listenSocket = SocketUtil::Create();

	int32 bindResult = SocketUtil::Bind(_listenSocket, NetworkAddress(INADDR_ANY, 20000));

	if (bindResult == SOCKET_ERROR)
	{
		cout << WSAGetLastError() << endl;
	}

	int32 listenResult = SocketUtil::Listen(_listenSocket);

	if (listenResult == SOCKET_ERROR)
	{
		cout << WSAGetLastError() << endl;
	}

}

void NetworkManager::ProcessNetworkIO()
{
	throughPut++;
	int32 cnt = 0;

	auto iter = SessionManager::Begin();

	while (true)
	{
		list<Session*> parts;

		FD_SET rset;
		FD_SET wset;
		FD_ZERO(&rset);
		FD_ZERO(&wset);

		FD_SET(_listenSocket, &rset);

		for (int32 idx = 0; idx < 63; idx++)
		{
			if (iter == SessionManager::End())
				break;

			Session* session = *iter;

			FD_SET(session->GetSock(), &rset);
			FD_SET(session->GetSock(), &wset);
			parts.push_back(session);

			++iter;
		}

		SelectSocket(parts, &rset, &wset);

		if (iter == SessionManager::End())
		{
			SessionManager::DisconnectAll();
			return;
		}
	}

	

}

void NetworkManager::SelectSocket(list<Session*> sessionTable, FD_SET* readSet, FD_SET* writeSet)
{
	timeval tv{ 0, 0 };
	int32 selectResult = ::select(0, readSet, writeSet, nullptr, &tv);

	if (selectResult > 0)
	{
		if (FD_ISSET(_listenSocket, readSet))
		{
			OnAccept();
		}

		for (Session* session : sessionTable)
		{
			if (session->IsDisconnect())
			{
				continue;
			}

			if (FD_ISSET(session->GetSock(), writeSet))
			{
				session->OnSend();
			}
		}

		for (Session* session : sessionTable)
		{
			if (session->IsDisconnect())
			{
				continue;
			}

			uint64 now = timeGetTime();
			if (now - session->GetTime() >= 30000)
			{
				SessionManager::ReserveDisconnect(session);
			}

		}


		for (Session* session : sessionTable)
		{
			if (session->IsDisconnect())
			{
				continue;
			}

			if (FD_ISSET(session->GetSock(), readSet))
			{
				session->OnRecv();
			}
		}
	}
}

bool NetworkManager::OnAccept()
{
	SOCKADDR_IN clientAddr;
	int32 addrLen = sizeof(clientAddr);

	SOCKET clientSocket = ::accept(_listenSocket, reinterpret_cast<SOCKADDR*>(&clientAddr), &addrLen);

	if (clientSocket == INVALID_SOCKET)
	{
		return false;
	}

	SocketUtil::SetLinger(clientSocket, 1, 0);

	Session* newSession = SessionManager::Create(clientSocket, clientAddr);

	Server::OnAccept(newSession);

	return true;
}
