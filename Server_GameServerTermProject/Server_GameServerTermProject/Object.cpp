#include "Object.h"

void Object::Init(int x, int y, int id, const char* name)
{
	_spawnPos.x = x;
	_spawnPos.y = y;
	_pos.x = x;
	_pos.y = y;
	_id = id;
	_level = rand() % 10 + 1;
	_hp = DEFALUT_MAX_HP * _level;
	strcpy_s(_name, name);
}

void Object::Attack(int target)
{

}
