#include "Object.h"

void Object::Init(int x, int y, int id, const char* name)
{
	_spawnPos.x = x;
	_spawnPos.y = y;
	_pos.x = x;
	_pos.y = y;
	_id = id;
	strcpy_s(_name, name);
}
