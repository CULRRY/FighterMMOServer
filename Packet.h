#pragma once
enum class PacketType : BYTE;
class RingBuffer;

struct PacketHeader
{
	BYTE code;
	BYTE size;
	PacketType type;
};

class Packet
{
	enum
	{
		BUFFER_DEFAULT_SIZE = 1400,
	};
public:
	Packet();
	Packet(int32 bufferSize);

	~Packet();

	void	Clear();

	int32	GetBufferSize() { return _bufferSize; };
	int32	GetDataSize() { return _writePos - _readPos; };

	BYTE*	GetBufferPtr() { return _buffer; }

	int32	MoveWritePos(int32 size);
	int32	MoveReadPos(int32 size);

	int32	GetData(BYTE* dest, int32 size);
	int32	PutData(BYTE* src, int32 size);

	void	MakeHeader(PacketType type);


	//-------------------------------------------------------------------------------
	// Operator Overloading
	//-------------------------------------------------------------------------------

	Packet& operator=(Packet& src);

	Packet& operator<<(BYTE value);
	Packet& operator<<(char value);
	Packet& operator<<(int16 value);
	Packet& operator<<(uint16 value);
	Packet& operator<<(int32 value);
	Packet& operator<<(uint32 value);
	Packet& operator<<(int64 value);
	Packet& operator<<(uint64 value);
	Packet& operator<<(float value);
	Packet& operator<<(double value);

	Packet& operator>>(BYTE& value);
	Packet& operator>>(char& value);
	Packet& operator>>(int16& value);
	Packet& operator>>(uint16& value);
	Packet& operator>>(int32& value);
	Packet& operator>>(uint32& value);
	Packet& operator>>(int64& value);
	Packet& operator>>(uint64& value);
	Packet& operator>>(float& value);
	Packet& operator>>(double& value);

protected:
	BYTE* _buffer;
	int32 _bufferSize;
	int32 _readPos;
	int32 _writePos;
	bool  _hasHeader;
};

