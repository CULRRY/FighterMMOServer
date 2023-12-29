#include "stdafx.h"
#include "RingBuffer.h"

RingBuffer::RingBuffer()
{
	_bufferSize = 5000;
	_allocSize = _bufferSize + 1;
	_front = 0;
	_rear = 0;

	_buffer = new BYTE[_allocSize];
	::memset(_buffer, 0, _allocSize);
}

RingBuffer::RingBuffer(int32 bufferSize)
{
	_bufferSize = bufferSize;
	_allocSize = _bufferSize + 1;
	_front = 0;
	_rear = 0;

	_buffer = new BYTE[_allocSize];
	::memset(_buffer, 0, _allocSize);
}

RingBuffer::~RingBuffer()
{
	delete _buffer;
}

void RingBuffer::Resize(int32 size)
{
	BYTE* newBuffer = new BYTE[size];
	::memcpy(newBuffer, &_buffer[_front], GetUseSize());
	delete _buffer;
	_buffer = newBuffer;

	_front = 0;
	_rear = GetUseSize();
}

int32 RingBuffer::GetBufferSize()
{
	return _bufferSize;
}

//-------------------------------------------------------------------------------
/// @brief 현제 사용중인 용량을 반환한다.
/// @return [int32] 사용중인 용량
//_______________________________________________________________________________
int32 RingBuffer::GetUseSize()
{
	if (_rear >= _front)
	{
		return _rear - _front;
	}
	else
	{
		return _bufferSize + _rear - _front + 1;
	}
}

//-------------------------------------------------------------------------------
/// @brief 현재 버퍼에 남은 용량을 반환한다.
/// @return [int32] 남은 용량
//_______________________________________________________________________________
int32 RingBuffer::GetFreeSize()
{
	if (_rear >= _front)
	{
		return _bufferSize - _rear + _front;
	}
	else
	{
		return _front - _rear - 1;
	}
}

//-------------------------------------------------------------------------------
/// @brief WritePos에 데이터를 넣는다.
///	@param data 데이터 포인터
/// @param size 데이터 크기
/// @return [int32] 넣은 크기
//_______________________________________________________________________________
int32 RingBuffer::Enqueue(BYTE* data, int size)
{
	if (size > GetFreeSize())
	{
		return -1;
	}

	if (_rear + size > _bufferSize)
	{
		int32 chunkSize = _allocSize - _rear;
		::memcpy(&_buffer[_rear], data, chunkSize);
		data += chunkSize;
		::memcpy(&_buffer[0], data, size - chunkSize);
	}
	else
	{
		::memcpy(&_buffer[_rear], data, size);
	}

	_rear = (_rear + size) % _allocSize;

	return size;
}

//-------------------------------------------------------------------------------
/// @brief ReadPos에서 데이터를 가져온다.
///	@param dest 데이터 포인터
/// @param size 데이터 크기
/// @return [int32] 가져온 크기
//_______________________________________________________________________________
int32 RingBuffer::Dequeue(BYTE* dest, int size)
{
	if (size > GetUseSize())
		return -1;

	if (_front + size > _bufferSize)
	{
		int32 chunkSize = _allocSize - _front;
		::memcpy(dest, &_buffer[_front], chunkSize);
		dest += chunkSize;
		::memcpy(dest, &_buffer[0], size - chunkSize);
	}
	else
	{
		::memcpy(dest, &_buffer[_front], size);
	}

	_front = (_front + size) % _allocSize;


	return size;
}

//-------------------------------------------------------------------------------
/// @brief ReadPos에서 데이터를 읽어온다. (ReadPos 고정)
///	@param dest 데이터 포인터
/// @param size 데이터 크기
/// @return [int32] 읽어온 크기
//_______________________________________________________________________________
int32 RingBuffer::Peek(BYTE* dest, int size)
{
	if (size > GetUseSize())
		return -1;

	if (_front + size > _bufferSize)
	{
		int32 chunkSize = _allocSize - _front;
		::memcpy(dest, &_buffer[_front], chunkSize);
		dest += chunkSize;
		::memcpy(dest, &_buffer[0], size - chunkSize);
	}
	else
	{
		::memcpy(dest, &_buffer[_front], size);
	}


	return size;
}

//-------------------------------------------------------------------------------
/// @brief 버퍼 포인터로 외부에서 한방에 쓸 수 있는 길이를 반환한다.
///	@details 원형큐의 구조상 버퍼의 끝단에 있는 데이터는 끝 -> 처음으로
///			  돌아가서 2번에 걸쳐서 데이터를 넣을 수 있다. 이 부분에서 끊어지지
///			  않은 길이를 의미한다.
/// @return  사용가능 용량
//_______________________________________________________________________________
int32 RingBuffer::DirectEnqueueSize()
{
	if (_rear >= _front)
	{
		if (_front == 0)
		{
			return _bufferSize - _rear;
		}
		else
		{
			return _bufferSize - _rear + 1;
		}
	}
	else
	{
		return _front - _rear - 1;
	}
}

//-------------------------------------------------------------------------------
/// @brief 버퍼 포인터로 외부에서 한방에 읽을 수 있는 길이를 반환한다.
///	@details 원형큐의 구조상 버퍼의 끝단에 있는 데이터는 끝 -> 처음으로
///			  돌아가서 2번에 걸쳐서 데이터를 읽을 수 있다. 이 부분에서 끊어지지
///			  않은 길이를 의미한다.
/// @return  사용가능 용량
//_______________________________________________________________________________
int32 RingBuffer::DirectDequeueSize()
{
	if (_rear >= _front)
	{
		return _rear - _front;
	}
	else
	{
		return _allocSize - _front;
	}
}

//-------------------------------------------------------------------------------
/// @brief 원하는 길이만큼 읽기위치에서 삭제한다.
/// @param size 이동할 길이
/// @return 이동한 길이
//_______________________________________________________________________________
int32 RingBuffer::MoveRear(int32 size)
{
	_rear = (_rear + size) % _allocSize;
	return size;
}

//-------------------------------------------------------------------------------
/// @brief 원하는 길이만큼 쓰기위치를 이동한다.
/// @param size 이동할 길이
/// @return 이동한 길이
//_______________________________________________________________________________
int32 RingBuffer::MoveFront(int32 size)
{
	_front = (_front + size) % _allocSize;
	return size;
}

//-------------------------------------------------------------------------------
/// @brief 버퍼에 있는 모든 데이터를 삭제한다.
//_______________________________________________________________________________
void RingBuffer::Clear()
{
	_front = 0;
	_rear = 0;
}
