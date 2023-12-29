#include "stdafx.h"
#include "Packet.h"

Packet::Packet()
	: _bufferSize(BUFFER_DEFAULT_SIZE), _readPos(0), _writePos(0), _hasHeader(false)
{
	_buffer = new BYTE[_bufferSize + sizeof(PacketHeader)];
	_buffer += sizeof(PacketHeader);
}

Packet::Packet(int32 bufferSize)
	:_bufferSize(bufferSize), _readPos(0), _writePos(0), _hasHeader(false)
{
	_buffer = new BYTE[_bufferSize + sizeof(PacketHeader)];
	_buffer += sizeof(PacketHeader);
}

Packet::~Packet()
{
	if (_hasHeader == false)
		_buffer -= sizeof(PacketHeader);
	delete[] _buffer;
}

void Packet::Clear()
{
	_readPos = 0;
	_writePos = 0;
}

int32 Packet::MoveWritePos(int32 size)
{
	_writePos += size;
	return size;
}

int32 Packet::MoveReadPos(int32 size)
{
	_readPos += size;
	return size;
}

int32 Packet::GetData(BYTE* dest, int32 size)
{
	if (_readPos + size > _writePos)
		return -1;

	::memcpy(dest, _buffer + _readPos, size);
	_readPos += size;

	return size;
}

int32 Packet::PutData(BYTE* src, int32 size)
{
	if (_writePos + size > _bufferSize)
		return -1;

	::memcpy(_buffer + _writePos, src, size);
	_writePos += size;

	return size;
}

void Packet::MakeHeader(PacketType type)
{
	if (_hasHeader == true)
		return;

	_hasHeader = true;
	_buffer -= sizeof(PacketHeader);
	_writePos += sizeof(PacketHeader);
	PacketHeader* header = reinterpret_cast<PacketHeader*>(_buffer);
	header->code = 0x89;
	header->type = type;
	header->size = static_cast<BYTE>(GetDataSize() - sizeof(PacketHeader));
}

Packet& Packet::operator=(Packet& src)
{
	return *this;
}

Packet& Packet::operator<<(BYTE value)
{
	PutData(&value, sizeof(BYTE));

	return *this;
}

Packet& Packet::operator<<(char value)
{
	PutData(reinterpret_cast<BYTE*>(&value), sizeof(char));

	return *this;
}

Packet& Packet::operator<<(int16 value)
{
	PutData(reinterpret_cast<BYTE*>(&value), sizeof(int16));

	return *this;
}

Packet& Packet::operator<<(uint16 value)
{
	PutData(reinterpret_cast<BYTE*>(&value), sizeof(uint16));

	return *this;
}

Packet& Packet::operator<<(int32 value)
{
	PutData(reinterpret_cast<BYTE*>(&value), sizeof(int32));

	return *this;
}

Packet& Packet::operator<<(uint32 value)
{
	PutData(reinterpret_cast<BYTE*>(&value), sizeof(uint32));

	return *this;
}

Packet& Packet::operator<<(int64 value)
{
	PutData(reinterpret_cast<BYTE*>(&value), sizeof(int64));

	return *this;
}

Packet& Packet::operator<<(uint64 value)
{
	PutData(reinterpret_cast<BYTE*>(&value), sizeof(uint64));

	return *this;
}

Packet& Packet::operator<<(float value)
{
	PutData(reinterpret_cast<BYTE*>(&value), sizeof(float));

	return *this;
}

Packet& Packet::operator<<(double value)
{
	PutData(reinterpret_cast<BYTE*>(&value), sizeof(double));

	return *this;
}

Packet& Packet::operator>>(BYTE& value)
{
	GetData(&value, sizeof(BYTE));

	return *this;
}

Packet& Packet::operator>>(char& value)
{
	GetData(reinterpret_cast<BYTE*>(&value), sizeof(BYTE));

	return *this;
}

Packet& Packet::operator>>(int16& value)
{
	GetData(reinterpret_cast<BYTE*>(&value), sizeof(int16));

	return *this;
}

Packet& Packet::operator>>(uint16 & value)
{
	GetData(reinterpret_cast<BYTE*>(&value), sizeof(uint16));

	return *this;
}

Packet& Packet::operator>>(int32 & value)
{
	GetData(reinterpret_cast<BYTE*>(&value), sizeof(int32));

	return *this;
}

Packet& Packet::operator>>(uint32 & value)
{
	GetData(reinterpret_cast<BYTE*>(&value), sizeof(uint32));

	return *this;
}

Packet& Packet::operator>>(int64 & value)
{
	GetData(reinterpret_cast<BYTE*>(&value), sizeof(int64));

	return *this;
}

Packet& Packet::operator>>(uint64 & value)
{
	GetData(reinterpret_cast<BYTE*>(&value), sizeof(uint64));

	return *this;
}

Packet& Packet::operator>>(float& value)
{
	GetData(reinterpret_cast<BYTE*>(&value), sizeof(float));

	return *this;
}

Packet& Packet::operator>>(double& value)
{
	GetData(reinterpret_cast<BYTE*>(&value), sizeof(double));

	return *this;
}
