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
/// @brief ���� ������� �뷮�� ��ȯ�Ѵ�.
/// @return [int32] ������� �뷮
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
/// @brief ���� ���ۿ� ���� �뷮�� ��ȯ�Ѵ�.
/// @return [int32] ���� �뷮
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
/// @brief WritePos�� �����͸� �ִ´�.
///	@param data ������ ������
/// @param size ������ ũ��
/// @return [int32] ���� ũ��
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
/// @brief ReadPos���� �����͸� �����´�.
///	@param dest ������ ������
/// @param size ������ ũ��
/// @return [int32] ������ ũ��
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
/// @brief ReadPos���� �����͸� �о�´�. (ReadPos ����)
///	@param dest ������ ������
/// @param size ������ ũ��
/// @return [int32] �о�� ũ��
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
/// @brief ���� �����ͷ� �ܺο��� �ѹ濡 �� �� �ִ� ���̸� ��ȯ�Ѵ�.
///	@details ����ť�� ������ ������ ���ܿ� �ִ� �����ʹ� �� -> ó������
///			  ���ư��� 2���� ���ļ� �����͸� ���� �� �ִ�. �� �κп��� ��������
///			  ���� ���̸� �ǹ��Ѵ�.
/// @return  ��밡�� �뷮
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
/// @brief ���� �����ͷ� �ܺο��� �ѹ濡 ���� �� �ִ� ���̸� ��ȯ�Ѵ�.
///	@details ����ť�� ������ ������ ���ܿ� �ִ� �����ʹ� �� -> ó������
///			  ���ư��� 2���� ���ļ� �����͸� ���� �� �ִ�. �� �κп��� ��������
///			  ���� ���̸� �ǹ��Ѵ�.
/// @return  ��밡�� �뷮
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
/// @brief ���ϴ� ���̸�ŭ �б���ġ���� �����Ѵ�.
/// @param size �̵��� ����
/// @return �̵��� ����
//_______________________________________________________________________________
int32 RingBuffer::MoveRear(int32 size)
{
	_rear = (_rear + size) % _allocSize;
	return size;
}

//-------------------------------------------------------------------------------
/// @brief ���ϴ� ���̸�ŭ ������ġ�� �̵��Ѵ�.
/// @param size �̵��� ����
/// @return �̵��� ����
//_______________________________________________________________________________
int32 RingBuffer::MoveFront(int32 size)
{
	_front = (_front + size) % _allocSize;
	return size;
}

//-------------------------------------------------------------------------------
/// @brief ���ۿ� �ִ� ��� �����͸� �����Ѵ�.
//_______________________________________________________________________________
void RingBuffer::Clear()
{
	_front = 0;
	_rear = 0;
}
