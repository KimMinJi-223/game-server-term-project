#pragma once
#include "protocol.h"

enum S_STATE { 
	ST_FREE, ST_ALLOC, ST_INGAME 
};
enum COMP_TYPE { 
	OP_ACCEPT, OP_RECV, OP_SEND, OP_NPC_MOVE, OP_AI_MOVE, OP_AI_LUA, OP_NPC_ATTACK, OP_RESPAWN,
	OP_LOGIN
};
enum EXEC_TYPE { 
	DBINIT, LOGINT, LOGOUT 
};
enum EVENT_TYPE { 
	EV_RANDOM_MOVE, EV_AI_MOVE, EV_AI_LUA, EV_CHASE, EV_HEAL, EV_ATTACK, EV_RESPAWN,
	EV_LOGIN
};
enum MONSTER_MOVE_STATE {
	MT_STOP, MT_RAND, MT_AI
};

constexpr int DEFALUT_EXP = 100;
constexpr int DEFALUT_MAX_HP = 100;

constexpr int VIEW_RANGE = 7;

constexpr int SECTOR_SIZE = 10;
constexpr int SECTOR_COUNT = (W_WIDTH / SECTOR_SIZE) * (W_HEIGHT / SECTOR_SIZE);
constexpr int ADJ_COUNT = 9;
constexpr int MULTIPLY_ROW = (SECTOR_COUNT / W_WIDTH * SECTOR_SIZE);
constexpr int adj_sector[9]{
	0,
	-(W_WIDTH / SECTOR_SIZE) - 1,
	-(W_WIDTH / SECTOR_SIZE),
	-(W_WIDTH / SECTOR_SIZE) + 1,
	-1,
	1,
	W_WIDTH / SECTOR_SIZE - 1,
	W_WIDTH / SECTOR_SIZE,
	W_WIDTH / SECTOR_SIZE + 1,
};

struct Pos
{
	int x;
	int y;

	bool operator==(Pos& other)
	{
		return y == other.y && x == other.x;
	}
	Pos operator+(const Pos& other) const
	{
		Pos ret;
		ret.y = y + other.y;
		ret.x = x + other.x;
		return ret;
	}
	bool operator<(const Pos& other) const
	{
		if (y != other.y)
			return y < other.y;
		return x < other.x;
	}
};

struct PQNode
{
	bool operator<(const PQNode& other) const { return f < other.f; }
	bool operator>(const PQNode& other) const { return f > other.f; }

	int f; // f = g + h
	int g;
	Pos pos;
};


