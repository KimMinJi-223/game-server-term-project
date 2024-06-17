#include "Monster.h"
#include "Server.h"
#include <iostream>

void Monster::Init(int id, int x, int y)
{
	if((id - MAX_USER) % 10000 == 0)
		printf("%d 마리\n", id - MAX_USER);
	
	if (id < NUM_NPC_1) {
		_monsterType = VI_MONSTER_1;
		_isAgro = false;
		_isRoaming = false;
		_roamingArea = 0;
		_power = 10;
	}
	else if (id < NUM_NPC_2) {
		_monsterType = VI_MONSTER_2;
		_isAgro = false;
		_isRoaming = true;
		_roamingArea = 10;
		_power = 10;

	}
	else if (id < NUM_NPC_3) {
		_monsterType = VI_MONSTER_3;
		_isAgro = true;
		_isRoaming = false;
		_roamingArea = 0;
		_power = 10;

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
	else if (id < NUM_NPC_4) {
		_monsterType = VI_MONSTER_4;
		_isAgro = true;
		_isRoaming = true;
		_roamingArea = 10;
		_power = 10;

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
	else  {
		_monsterType = VI_MONSTER_4;
		_isAgro = true;
		_isRoaming = true;
		_roamingArea = 10;
		_power = 10;

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
	//printf("move\n");
	// 단순 영역안에서 이동이면 서버에서 하자. 
	// 맞았나, 타겟이 범위에 있나 확인은 루아에서하자
	// 이 이동은 서버에서 다른거는 클라에서

	// 서버에서 움직이는 상태
	if (!_is_AI_move)
		Server::GetInstance()->GetTImer()->add_timer(_id,-1, EV_RANDOM_MOVE, 1000);

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

		if (!(Server::GetInstance()->can_go(x, y))) {
			continue;;
		}
		break;
	}
	SetPosition(x, y);
}

bool Monster::CASIsActive(bool expect, bool update)
{
	bool input = expect;
	return atomic_compare_exchange_strong(&_is_active, &input, update);
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

void Monster::AddTimer(int id)
{
	lua_lock.lock();
	lua_getglobal(_L, "add_timer");
	lua_pushnumber(_L, id);
	int error = lua_pcall(_L, 1, 0, 0);
	if (error) {
		//std::cout << "Error:" << lua_tostring(_L, -1);
		lua_pop(_L, 1);
		lua_lock.unlock();
		
		return ;
	}
	lua_lock.unlock();
}

void Monster::IsAStar(int playerId, int x, int y)
{
	lua_lock.lock();
	lua_getglobal(_L, "IsAStar");
	lua_pushnumber(_L, playerId);
	lua_pushnumber(_L, x);
	lua_pushnumber(_L, y);
	int error = lua_pcall(_L, 3, 0, 0);
	if (error) {
		std::cout << "IsAStar Error:" << lua_tostring(_L, -1);
		lua_pop(_L, 1);
	lua_lock.unlock();
		return;
	}
	lua_lock.unlock();

}
