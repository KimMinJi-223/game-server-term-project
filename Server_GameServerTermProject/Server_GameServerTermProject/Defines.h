#pragma once
#include "protocol.h"

enum S_STATE { ST_FREE, ST_ALLOC, ST_INGAME };
enum COMP_TYPE { OP_ACCEPT, OP_RECV, OP_SEND, OP_NPC_MOVE, OP_AI_MOVE, OP_AI_HELLO };

enum EVENT_TYPE { EV_RANDOM_MOVE, EV_AI_MOVE, EV_CHASE, EV_HEAL, EV_ATTACK };

constexpr int VIEW_RANGE = 5;

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

};