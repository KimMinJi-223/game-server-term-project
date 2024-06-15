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
	_maxHp = _hp;
	strcpy_s(_name, name);
}

void Object::Attack(int target)
{

}

int Object::Damage(int damageValue, bool& isSuccess)
{
	while (true) {
		int expect = _hp;
		if (_hp <= 0) {
			isSuccess = false;
			return _hp; // 이미 죽은 몬스터임
		}
		int update = _hp - damageValue;
		if (atomic_compare_exchange_strong(reinterpret_cast<std::atomic_int*>(&_hp), &expect, update))
			break;
	}
	if (_hp <= 0)
		_hp = 0;
	isSuccess = true;
	return _hp;
}

bool Object::SetExp(int exp)
{
	bool isLevelUp = false;
	_exp = exp + _exp; 
	while (true) { 
		if (_exp > _maxExp) {
			isLevelUp = true;
			_exp %= _maxExp;
			++_level;
			_maxExp = DEFALUT_EXP * pow(2, _level - 1);;
		}
		else return isLevelUp;
	}
}