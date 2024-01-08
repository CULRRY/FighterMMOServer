#pragma once
#include "NetworkAddress.h"
#include "ObjectPool.h"
#include "RingBuffer.h"

class Session
{
	inline static uint32 idGenerator = 0;
	inline static ObjectPool<RingBuffer> bufferPool{20000, false};
public:
	Session() = default;
	Session(SOCKET sock, SOCKADDR_IN sockAddr);
	~Session();

	bool OnRecv();
	bool OnSend();
	bool OnDisconnect();

	bool IsDisconnect()
	{
		return _isReservedDisconnect;
	}

	uint32 GetId() { return _sessionId; }
	SOCKET GetSock() { return _socket; }
	RingBuffer& GetRecvBuffer(){ return *_recvBuffer; }
	RingBuffer& GetSendBuffer(){ return *_sendBuffer; }

	uint64 GetTime() { return _lastRecvTime; }
	void SetTime(uint64 time) { _lastRecvTime = time; }


private:
	uint32			_sessionId;
	SOCKET			_socket;
	NetworkAddress	_netInfo;
	RingBuffer*		_recvBuffer;
	RingBuffer*		_sendBuffer;
	uint64			_lastRecvTime;
	bool			_isReservedDisconnect;
};

