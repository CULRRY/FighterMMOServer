#pragma once
#include <list>

class Session;
class NetworkManager
{

public:
	static void Init();
	static void ProcessNetworkIO();
	static void SelectSocket(list<Session*> sessionTable, FD_SET* readSet, FD_SET* writeSet);

	static bool OnAccept();


private:
	inline static SOCKET _listenSocket;
};

