#pragma once

enum class Direction : BYTE;
class Packet;
class Session;

enum class PacketType : BYTE
{
	S_CREATE_MY_CHARACTER = 0,
	S_CREATE_OTHER_CHARACTER = 1,
	S_DELETE_CHARACTER = 2,
	C_MOVE_START = 10,
	S_MOVE_START = 11,
	C_MOVE_STOP = 12,
	S_MOVE_STOP = 13,
	C_ATTACK1 = 20,
	S_ATTACK1 = 21,
	C_ATTACK2 = 22,
	S_ATTACK2 = 23,
	C_ATTACK3 = 24,
	S_ATTACK3 = 25,
	S_DAMAGE = 30,
	S_SYNC = 251,
	C_ECHO = 252,
	S_ECHO = 253,
};


class PacketHandler
{
public:
	static bool HandlePacket(Session* session);


	static bool Handle_C_MOVE_START(Session* session, Direction& dir, int16 x, int16 y);
	static bool Handle_C_MOVE_STOP(Session* session, Direction& dir, int16 x, int16 y);
	static bool Handle_C_ATTACK1(Session* session, Direction& dir, int16 x, int16 y);
	static bool Handle_C_ATTACK2(Session* session, Direction& dir, int16 x, int16 y);
	static bool Handle_C_ATTACK3(Session* session, Direction& dir, int16 x, int16 y);
	static bool Handle_C_ECHO(Session* session, uint32 time);
	//////////////////
	/// MakePacket ///
	//////////////////
	static bool Make_S_CREATE_MY_CHARACTER(Packet& pkt, int32 id, Direction& dir, int16 x, int16 y, int8 hp);
	static bool Make_S_CREATE_OTHER_CHARACTER(Packet& pkt, int32 id, Direction& dir, int16 x, int16 y, int8 hp);
	static bool Make_S_DELETE_CHARACTER(Packet& pkt, int32 id);
	static bool Make_S_MOVE_START(Packet& pkt, int32 id, Direction& dir, int16 x, int16 y);
	static bool Make_S_MOVE_STOP(Packet& pkt, int32 id, Direction& dir, int16 x, int16 y);
	static bool Make_S_ATTACK1(Packet& pkt, int32 id, Direction& dir, int16 x, int16 y);
	static bool Make_S_ATTACK2(Packet& pkt, int32 id, Direction& dir, int16 x, int16 y);
	static bool Make_S_ATTACK3(Packet& pkt, int32 id, Direction& dir, int16 x, int16 y);
	static bool Make_S_DAMAGE(Packet& pkt, int32 attackId, int32 damageId, int8 damageHp);
	static bool Make_S_SYNC(Packet& pkt, int32 id, int16 x, int16 y);
	static bool Make_S_ECHO(Packet& pkt, uint32 time);
};

