#pragma once
#include <map>

#include "Session.h"
#include "ObjectPool.h"
#include "Character.h"

class Session;

class Server
{
public:
	static constexpr int32 MAX_WIDTH = 640;
	static constexpr int32 MAX_HEIGHT = 480;

	static constexpr int32 RANGE_MOVE_TOP = 0;
	static constexpr int32 RANGE_MOVE_LEFT = 0;
	static constexpr int32 RANGE_MOVE_RIGHT = 6400;
	static constexpr int32 RANGE_MOVE_BOTTOM = 6400;

	static constexpr int32 ATTACK1_RANGE_X = 80;
	static constexpr int32 ATTACK2_RANGE_X = 90;
	static constexpr int32 ATTACK3_RANGE_X = 100;
	static constexpr int32 ATTACK1_RANGE_Y = 10;
	static constexpr int32 ATTACK2_RANGE_Y = 10;
	static constexpr int32 ATTACK3_RANGE_Y = 20;
	static constexpr int8 ATTACK1_DAMAGE = 1;
	static constexpr int8 ATTACK2_DAMAGE = 2;
	static constexpr int8 ATTACK3_DAMAGE = 3;
	inline static int32 shutDown = true;
	
public:
	static void Init();
	static void Run();
	static bool OnAccept(Session* session);
	static bool Update();
	static Character* FindCharacter(Session* session)
	{
		auto it = _characterMap.find(session->GetId());

		if (it == _characterMap.end())
		{
			return nullptr;
		}
		else
		{
			return it->second;
		}
	}
	static void DeleteCharacter(Session* session);

	inline static uint32 networkTime = 0;

	static bool IsAttackRange(Character* character, Character* target, int32 rangeX, int32 rangeY);
	inline static map<uint32, Character*> _characterMap;
	inline static ObjectPool<Character> _characterPool{10000, true};
	inline static int32 _acceptCount = 0;
	inline static uint32 _logicTime = 0;
	inline static uint32 _globalTime = 0;
	inline static uint64 _frame = 0;
	inline static uint64 _frameTime = 0;
	inline static int32 _deathCount = 0;
	inline static int32 _totalDeath = 0;
};

