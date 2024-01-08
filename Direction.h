#pragma once
#include "Packet.h"

enum class Direction : BYTE
{
	LL,
	LU,
	UU,
	RU,
	RR,
	RD,
	DD,
	LD,
};

inline Packet& operator<<(Packet& pkt, Direction dir)
{
	pkt.PutData(reinterpret_cast<BYTE*>(&dir), sizeof(Direction));
	return pkt;
}

inline Packet& operator>>(Packet& pkt, Direction& dir)
{
	pkt.GetData(reinterpret_cast<BYTE*>(&dir), sizeof(Direction));
	return pkt;
}

inline extern int32 dx[] = { -1, -1, 0, 1, 1, 1, 0, -1 };
inline extern int32 dy[] = { 0, -1, -1, -1, 0, 1, 1, 1 };
