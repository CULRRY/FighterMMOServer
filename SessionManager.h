#pragma once
#include <list>

#include "ObjectPool.h"
#include "Session.h"

class Packet;

class SessionManager
{
	inline static ObjectPool<Session> sessionPool{5000, true};

public:
	static void Init();
	static Session* Create(SOCKET sock, SOCKADDR_IN sockAddr);
	static void ReserveDisconnect(Session* session);

	static void SendUnicast(Session* session, Packet& pkt);
	static void DisconnectAll();
	static int32 Count() { return _sessionList.size(); }

	static list<Session*>::iterator Begin() { return _sessionList.begin(); }
	static list<Session*>::iterator End() { return _sessionList.end(); }


private:
	inline static list<Session*> _sessionList;
};

