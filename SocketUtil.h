#pragma once
#include "NetworkAddress.h"

class SocketUtil
{
public:
	static int32	Init();
	static void		CleanUp();

	static SOCKET	Create();
	static int32	Close(SOCKET socket);

	static bool		SetLinger(SOCKET socket, uint16 onoff, uint16 linger);
	static bool		SetRecvBufferSize(SOCKET socket, int32 size);
	static bool		SetSendBufferSize(SOCKET socket, int32 size);
	static bool		SetTcpNoDelay(SOCKET socket, bool flag);

	static bool		Bind(SOCKET socket, NetworkAddress addr);
	static bool		Listen(SOCKET socket, int32 backlog = SOMAXCONN);
	static bool		Connect(SOCKET socket, NetworkAddress addr);
};