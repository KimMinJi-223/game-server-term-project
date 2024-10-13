#include "Monster.h"
#include "Server.h"
#include <iostream>

void Monster::Init(int id, int x, int y)
{
	if((id - MAX_USER) % 10000 == 0)
		printf("%d 마리\n", id - MAX_USER);
	
	if (id < NUM_NPC_1) 
		SetMonsterType(VI_MONSTER_1);
	else if (id < NUM_NPC_2)
		SetMonsterType(VI_MONSTER_2);
	else if (id < NUM_NPC_3)
		SetMonsterType(VI_MONSTER_3);
	else if (id < NUM_NPC_4)
		SetMonsterType(VI_MONSTER_4);
	else 
		SetMonsterType(VI_MONSTER_4);


	if (_monsterType == VI_MONSTER_3 || _monsterType == VI_MONSTER_4) {
		_L = luaL_newstate();
		luaL_openlibs(_L);
		luaL_loadfile(_L, "monster.lua");
		lua_pcall(_L, 0, 0, 0);

		lua_register(_L, "API_GetPosX", Server::API_GetPosX);
		lua_register(_L, "API_GetPosY", Server::API_GetPosY);
		lua_register(_L, "API_AStarStart", Server::API_AStarStart);
		lua_register(_L, "API_AStarEnd", Server::API_AStarEnd);
		lua_register(_L, "API_AddTimer", Server::API_AddTimer);

		lua_getglobal(_L, "set_init");
		lua_pushnumber(_L, id);
		lua_pushnumber(_L, x);
		lua_pushnumber(_L, y);
		lua_pushnumber(_L, _monsterType);
		lua_pcall(_L, 4, 0, 0);
	}
	
	char name[NAME_SIZE];
	sprintf_s(name, "M%d", id);
	Object::Init(x, y, id, name);
	SetIsNpc(true);
}

void Monster::move(int& x, int& y)
{
	// 루아에서 플레이어를 추적하라는 상태로 바꾸면 EV_RANDOM_MOVE이벤트를 등록하지 않는다. 
	if (!_isAiMove) 
		Server::GetInstance()->GetTImer()->AddTaskTimer(_id,-1, EV_RANDOM_MOVE, 1000);
	
	while (true) {
		x = _pos.x;
		y = _pos.y;
		switch (rand() % 4) {
		case DIR_UP: if (y > 0) y--; _dir = DIR_UP; break;
		case DIR_DOWN: if (y < W_HEIGHT - 1) y++; _dir = DIR_DOWN; break;
		case DIR_LEFT: if (x > 0) x--; _dir = DIR_LEFT; break;
		case DIR_RIGHT: if (x < W_WIDTH - 1) x++; _dir = DIR_RIGHT; break;
		}

		if ((_spawnPos.x - x) * (_spawnPos.x - x) > _roamingArea * _roamingArea)
			continue;
		if ((_spawnPos.y - y) * (_spawnPos.y - y) > _roamingArea * _roamingArea)
			continue;

		if (!(Server::GetInstance()->CanGo(x, y))) {
			continue;;
		}
		break;
	}
	SetPosition(x, y);
}

bool Monster::CASIsActive(bool expect, bool update)
{
	bool input = expect;
	return atomic_compare_exchange_strong(&_isActive, &input, update);
}

int Monster::GetExpOnDeath()
{
	int exp = _level * _level * 2;
	if (_isAgro)
		exp *= 2;
	if (_isRoaming)
		exp *= 2;
	return exp;
}

void Monster::SetMonsterType(int type) {
	_monsterType = type;
	if (type == VI_MONSTER_3 || type == VI_MONSTER_4)
		_isAgro = true;
	else
		_isAgro = false;

	if (type == VI_MONSTER_2 || type == VI_MONSTER_4) {
		_roamingArea = 10;
		_isRoaming = true;
	}
	else {
		_roamingArea = 0;
		_isRoaming = false;
	}
}

void Monster::isDoAStar(int causeId, int x, int y)
{
	luaLock.lock();
	lua_getglobal(_L, "isDoAStar");
	lua_pushnumber(_L, causeId);
	lua_pushnumber(_L, x);
	lua_pushnumber(_L, y);
	int error = lua_pcall(_L, 3, 0, 0);
	if (error) {
		std::cout << "IsAStar Error:" << lua_tostring(_L, -1);
		lua_pop(_L, 1);
		luaLock.unlock();
		return;
	}
	luaLock.unlock();
}
