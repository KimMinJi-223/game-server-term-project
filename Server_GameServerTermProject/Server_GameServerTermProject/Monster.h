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

public:
	bool GetIsActive() { return _is_active; }
	void SetIsActive(bool newActive) { _is_active = newActive; }
	bool CASIsActive(bool expect, bool update);
};

