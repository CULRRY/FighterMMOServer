#pragma once
#include <unordered_map>

class Packet;
class Session;
class Character;

class Sector
{
	friend Character;
public:
	static constexpr int32 WIDTH = 160;
	static constexpr int32 HEIGHT = 160;

	Sector() {}
	Sector(int y, int32 x) : _y(y), _x(x) {}

	void Insert(Character* session);
	void Delete(Character* session);
	void Move(Character* session, Sector* sector);
	void Broadcast(Session* except, Packet& pkt);

	unordered_map<uint32, Character*>::iterator Begin()
	{
		return _sessionList.begin();
	}
	unordered_map<uint32, Character*>::iterator End()
	{
		return _sessionList.end();
	}

	int16 GetY() { return _y; }
	int16 GetX() { return _x; }

private:
	int16 _y;
	int16 _x;
	unordered_map<uint32, Character*> _sessionList;
};

