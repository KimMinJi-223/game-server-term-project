#pragma once
#include "object.h"
#include <mutex>
#include "include/lua.hpp"

#pragma comment(lib, "lua54.lib")

class Monster : public Object
{
private:
	std::chrono::system_clock::time_point _npc_move_time;
	lua_State* _L;
	std::mutex lua_lock;
	bool _is_AI_move;
	std::atomic<bool>	_is_active;
	int _monsterType; // 데이터 레이스 없음
	bool _isRoaming; // 데이터 레이스 없음
	bool _isAgro; // 데이터 레이스 없음
	short _roamingArea; 
	int _targetId;

public:
	void Init(int id, int x, int y);
	void move(int& x, int& y);
	void AddTimer(int id);
	void IsAStar(int PlayerId, int x, int y);
public:
	bool GetIsActive() { return _is_active; }
	void SetIsActive(bool newActive) { _is_active = newActive; }
	bool CASIsActive(bool expect, bool update);
	bool GetISAIMove() { return _is_AI_move; }
	void SetISAIMove(bool isMove) { _is_AI_move = isMove; }
	void SetTarget(int target) { _targetId = target; }
	int GerTarget() { return _targetId; }

	void SetMonsterType(char type) { _monsterType = type; }
	int GetMonsterType() { return _monsterType; }

	bool GetIsRoaming() { return _isRoaming; }
	bool GetIsAgro() { return _isAgro; }

};