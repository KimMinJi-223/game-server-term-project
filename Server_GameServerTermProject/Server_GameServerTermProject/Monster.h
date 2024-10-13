#pragma once
#include "object.h"
#include <mutex>
#include "include/lua.hpp"
#include <iostream>
#pragma comment(lib, "lua54.lib")

class Monster : public Object
{
private:
	lua_State* _L;
	std::mutex luaLock;
	bool _isAiMove;
	std::atomic<bool>	_isActive;
	int _monsterType;
	bool _isRoaming;
	bool _isAgro;
	short _roamingArea; 
	int _targetId;

public:
	void Init(int id, int x, int y);
	void move(int& x, int& y);
	void isDoAStar(int causeId, int x, int y);

public:
	bool GetIsActive() { return _isActive; }
	void SetIsActive(bool newActive) { _isActive = newActive; }
	bool CASIsActive(bool expect, bool update);

	bool GetISAIMove() { return _isAiMove; }
	void SetISAIMove(bool isMove) { _isAiMove = isMove; }

	void SetTarget(int target) { _targetId = target; }
	int GerTarget() { return _targetId; }

	int GetMonsterType() { return _monsterType; }

	bool GetIsRoaming() { return _isRoaming; }
	bool GetIsAgro() { return _isAgro; }

	int GetExpOnDeath();

private:
	void SetMonsterType(int type);
};