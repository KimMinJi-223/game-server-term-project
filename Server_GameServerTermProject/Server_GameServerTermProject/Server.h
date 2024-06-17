#pragma once
#include <WS2tcpip.h>
#include <MSWSock.h>
#include "Defines.h"
#include "protocol.h"
#include <array>
#include "Object.h"
#include "OVER_EXP.h"
#include "Sector.h"
#include "Timer.h"
#include "DB.h"

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

#include "include/lua.hpp"
#pragma comment(lib, "lua54.lib")

class Session;

class Server
{
private:
	Server() { }
public:
	static Server* GetInstance()
	{
		static Server s_instance;
		return &s_instance;
	}

private:
	SOCKET _listen_socket;
	SOCKET _client_socket;
	OVER_EXP _accept_over;

	HANDLE _hiocp;

	std::array<Object*, MAX_USER + NUM_NPC> objects;
	std::array<Sector, SECTOR_COUNT> sectors;

	Timer _timerQueue;
	DB _dbQueue;

	std::vector<std::vector<int>> _collision;

public:
	void Init();
	void initialize_npc();
	void LoadCollision(const char* fileName);

public:
	void process_packet(int id, char* packet);
	void WorkerThread();
	void disconnect(int key);
	void BroadCastChat(int id, char* packet);

	void process_move(Session* movePlayer, int id, char direction);
	void GetNearPlayersList(int id, std::unordered_set<int>& list);
	int SetSectorId(Object& obj, int id, int x, int y);

public:
	void AStar(int& x, int& y, int id);
	int FindAttackedMonster(int id);
	bool FindASkillMonster(int id, int* ids);
public:
	int get_new_client_id();
	bool can_see(int a, int b);
	bool can_go(int x, int y);
	Timer* GetTImer() { return &_timerQueue; }
	DB* GetDB() { return &_dbQueue; }

public:
	static int API_GetPosX(lua_State* L);
	static int API_GetPosY(lua_State* L);
	static int API_AStarStart(lua_State* L);
	static int API_AStarEnd(lua_State* L);
	static int API_AddTimer(lua_State* L);
};
