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