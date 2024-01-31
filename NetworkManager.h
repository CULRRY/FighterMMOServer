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
	inline static int32 throughPut = 0;
	inline static int32 timeOutCount = 0;
	inline static uint32 selectTime = 0;
	inline static uint32 acceptTime = 0;
	inline static uint32 sendTime = 0;
	inline static uint32 recvTime = 0;

private:
	inline static SOCKET _listenSocket;
	
};

