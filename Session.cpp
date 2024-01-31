#include "stdafx.h"
#include "Session.h"

#include "Log.h"
#include "NetworkManager.h"
#include "RingBuffer.h"
#include "ObjectPool.h"
#include "PacketHandler.h"
#include "SessionManager.h"
#include "SocketUtil.h"

Session::Session(SOCKET sock, SOCKADDR_IN sockAddr)
	: _sessionId(idGenerator++), _socket(sock), _netInfo(sockAddr), _recvBuffer(bufferPool.Alloc(50000)), _sendBuffer(bufferPool.Alloc(50000)), _lastRecvTime(timeGetTime()),
	_isReservedDisconnect(false)
{
	_sendBuffer->Clear();
	_recvBuffer->Clear();
}

Session::~Session()
{
	bufferPool.Free(_recvBuffer);
	bufferPool.Free(_sendBuffer);
	SocketUtil::Close(_socket);
}

bool Session::OnRecv()
{

	int32 recvResult = ::recv(
		_socket,
		reinterpret_cast<char*>(_recvBuffer->GetRearBufferPtr()),
		_recvBuffer->DirectEnqueueSize(),
		0
	);

	if (recvResult == 0)
	{
		SessionManager::ReserveDisconnect(this);
		return false;
	}

	if (recvResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			//LOG_ERROR(L"Recv Error : {}", WSAGetLastError());
			SessionManager::ReserveDisconnect(this);
			return false;
		}
	}

	_recvBuffer->MoveRear(recvResult);

	while (PacketHandler::HandlePacket(this))
	{
		NetworkManager::throughPut++;
	}

	return true;
}

bool Session::OnSend()
{
	int32 sendResult = ::send(
		_socket,
		(char*)_sendBuffer->GetFrontBufferPtr(),
		_sendBuffer->DirectDequeueSize(),
		0
	);

	if (sendResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
		//	cout << WSAGetLastError() << endl;
			//LOG_ERROR(L"Send Error : {}", WSAGetLastError());
			SessionManager::ReserveDisconnect(this);
		}
	}

	_sendBuffer->MoveFront(sendResult);

	return true;
}

bool Session::OnDisconnect()
{
	_isReservedDisconnect = true;
	return true;
}
