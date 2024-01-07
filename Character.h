#pragma once
class Sector;
enum class Direction : BYTE;
enum class Action;
class Session;

class Character
{
public:
	

	Character(Session* session);

	void UpdateSector();
	void Move();
	bool IsAlive()
	{
		if (_hp <= 0)
		{
			return false;
		}

		return true;
	}

	void SetAction(Action action)
	{
		_action = action;
	}

	void SetPosition(int16 y, int16 x)
	{
		_x = x;
		_y = y;
	}

	Session* GetSession() const
	{
		return _session;
	}

	int32 GetSessionId() const
	{
		return _sessionId;
	}

	Direction& GetDir()
	{
		return _dir;
	}

	Direction& GetMoveDir()
	{
		return _moveDir;
	}

	int16 GetX() const
	{
		return _x;
	}

	int16 GetY() const
	{
		return _y;
	}

	Sector* GetSector() const
	{
		return _sector;
	}

	uint8 GetHp() const
	{
		return _hp;
	}

private:
	Session*	_session;
	int32		_sessionId;
	Action		_action;

	Direction	_dir;
	Direction	_moveDir;
	int16		_x;
	int16		_y;
	Sector*		_sector;

	uint8		_hp;

	
};

