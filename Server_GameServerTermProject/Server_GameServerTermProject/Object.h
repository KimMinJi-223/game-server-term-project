#pragma once
#include "Defines.h"
#include "protocol.h"
#include <mutex>

class Object
{
protected:
	Pos _pos;
	int _id;
	char _name[NAME_SIZE];
	S_STATE _state;
	std::mutex _s_lock;
	int _sectorId;
	bool	_is_npc;

public:
	void Init(int x, int y, int id, const char* name);

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
};

