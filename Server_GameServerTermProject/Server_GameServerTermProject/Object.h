#pragma once
#include "Defines.h"
#include "protocol.h"
#include <mutex>

class Object
{
protected:
	Pos _spawnPos;
	Pos _pos;
	int _id;
	char _name[NAME_SIZE];
	S_STATE _state;
	std::mutex _s_lock;
	int _dir;
	int _sectorId;
	bool	_is_npc;

	int _hp;
	int	_maxHp;
	int	_exp;
	int	_maxExp;
	int	_level;
	int _power;

public:
	// 스트레스 테스트
	int		_last_move_time_stress_test;

public:
	void Init(int x, int y, int id, const char* name);

public:
	void Attack(int target);
	int Damage(int damageValue, bool& isSuccess); // 체력을 반환

public:
	std::mutex& GetStateMutex() { return _s_lock; }
	S_STATE GetState() { return _state; }
	void SetState(S_STATE newState) { _state = newState; }

	void SetPosition(int x, int y) { _pos.x = x, _pos.y = y; }
	Pos GetPosition() { return _pos; }
	void SetName(const char* name) { strcpy_s(_name, name); }
	char* GetName() { return _name; }
	int GetId() { return _id; }
	void SetIsNpc(bool is_npc) { _is_npc = is_npc; }
	bool GetIsNpc() { return _is_npc; }
	void SetSectorId(int sectorId) { _sectorId = sectorId; }
	int GetSectorId() { return _sectorId; }
	void SetDir(int dir) { _dir = dir; }
	int GetDir() { return _dir; }
	void SetLevel(int level) { _level = level; }
	int GetLevel() { return _level; }
	bool SetExp(int exp); // 반환값은 레벨업 유무
	int GetExp() { return _exp; }
	void SetHp(int hp) { _hp = hp; }
	int GetHp() { return _hp; }
	void SetMaxHp(int hp) { _maxHp = hp; }
	int GetMaxHp() { return _maxHp; }
	void SetSpawnPos(Pos sPos) { _spawnPos = sPos; }
	Pos GetSpawnPos() { return _spawnPos; }
	void SetPower(int power) { _power = power; }
	int GetPower() { return _power; }
	virtual int GetExpOnDeath() { return 0; }
};

