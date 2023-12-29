#include "stdafx.h"
#include "SocketUtil.h"


int32 SocketUtil::Init()
{
	WSADATA wsa;
	return WSAStartup(MAKEWORD(2, 2), &wsa);
}

void SocketUtil::CleanUp()
{
	WSACleanup();
}

//-------------------------------------------------------------------------------
/// @brief TCP ������ �����Ѵ�.
/// @return ������ TCP ����
//-------------------------------------------------------------------------------
SOCKET SocketUtil::Create()
{
	SOCKET ret = ::socket(AF_INET, SOCK_STREAM, NULL);
	u_long on = 1;
	ioctlsocket(ret, FIONBIO, &on);
	return ret;
}

//-------------------------------------------------------------------------------
/// @brief ������ ��ȯ�Ѵ�.
/// @param socket ��ȯ�� ����
/// @return �Լ� ȣ�� ���
//-------------------------------------------------------------------------------
int32 SocketUtil::Close(SOCKET socket)
{
	return closesocket(socket);
}

//-------------------------------------------------------------------------------
/// @brief Linger ����
//-------------------------------------------------------------------------------
bool SocketUtil::SetLinger(SOCKET socket, uint16 onoff, uint16 linger)
{
	LINGER option{ onoff, linger };
	return SOCKET_ERROR != 
		setsockopt(
			socket, 
			SOL_SOCKET, 
			SO_LINGER, 
			reinterpret_cast<char*>(&option), 
			sizeof(option)
		);
}

//-------------------------------------------------------------------------------
/// @brief L4 ���Ź��� ����� �����Ѵ�.
//-------------------------------------------------------------------------------
bool SocketUtil::SetRecvBufferSize(SOCKET socket, int32 size)
{
	return SOCKET_ERROR !=
		setsockopt(
			socket,
			SOL_SOCKET,
			SO_RCVBUF,
			reinterpret_cast<char*>(&size),
			sizeof(size)
		);
}

//-------------------------------------------------------------------------------
/// @brief L4 �۽Ź��� ����� �����Ѵ�.
//-------------------------------------------------------------------------------
bool SocketUtil::SetSendBufferSize(SOCKET socket, int32 size)
{
	return SOCKET_ERROR !=
		setsockopt(
			socket,
			SOL_SOCKET,
			SO_SNDBUF,
			reinterpret_cast<char*>(&size),
			sizeof(size)
		);
}

//-------------------------------------------------------------------------------
/// @brief Nagle �˰��� on/off �����Ѵ�.
//-------------------------------------------------------------------------------
bool SocketUtil::SetTcpNoDelay(SOCKET socket, bool flag)
{
	return SOCKET_ERROR !=
		setsockopt(
			socket,
			SOL_SOCKET,
			TCP_NODELAY,
			reinterpret_cast<char*>(&flag),
			sizeof(flag)
		);
}

bool SocketUtil::Bind(SOCKET socket, NetworkAddress addr)
{
	SOCKADDR_IN sockAddr = addr.GetSockAddr();
	return SOCKET_ERROR !=
		::bind(socket, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(addr));
}

bool SocketUtil::Listen(SOCKET socket, int32 backlog)
{
	return SOCKET_ERROR !=
		::listen(socket, backlog);
}

bool SocketUtil::Connect(SOCKET socket, NetworkAddress addr)
{
	SOCKADDR_IN sockAddr = addr.GetSockAddr();
	return SOCKET_ERROR !=
		::connect(socket, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(addr));
}
