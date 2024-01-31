#pragma once
#include <unordered_map>

class Packet;
class Session;
class Character;

class Sector
{
	friend Character;
public:
	static constexpr int32 WIDTH = 100;
	static constexpr int32 HEIGHT = 100;

	Sector() {}
	Sector(int16 y, int16 x);

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

	vector<Sector*>& Around() { return _sectorsAround; }
	vector<Sector*>& Left() { return _sectorsLeft; }
	vector<Sector*>& Right() { return _sectorsRight; }
	vector<Sector*>& Top() { return _sectorsTop; }
	vector<Sector*>& Bottom() { return _sectorsBottom; }
	vector<Sector*>& LeftTop() { return _sectorsLeftTop; }
	vector<Sector*>& RightTop() { return _sectorsRightTop; }
	vector<Sector*>& LeftBottom() { return _sectorsLeftBottom; }
	vector<Sector*>& RightBottom() { return _sectorsRightBottom; }



private:
	int16 _y;
	int16 _x;
	unordered_map<uint32, Character*> _sessionList;

	vector<Sector*>	_sectorsAround;
	vector<Sector*>	_sectorsLeft;
	vector<Sector*>	_sectorsRight;
	vector<Sector*>	_sectorsTop;
	vector<Sector*>	_sectorsBottom;
	vector<Sector*>	_sectorsLeftTop;
	vector<Sector*>	_sectorsRightTop;
	vector<Sector*>	_sectorsLeftBottom;
	vector<Sector*>	_sectorsRightBottom;
};

