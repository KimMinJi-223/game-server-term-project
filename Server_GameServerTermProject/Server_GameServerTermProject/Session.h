#pragma once
#include "Defines.h"
#include "Object.h"
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <unordered_set>
#include <mutex>
#include "OVER_EXP.h"
#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

class Session : public Object
{
private:
	SOCKET _socket;
	OVER_EXP _recv_over;
	std::unordered_set <int> _view_list;
	std::mutex	_vll;

public:
	std::chrono::system_clock::time_point _last_attak_time;
	std::chrono::system_clock::time_point _last_move_time;

public:
	void Init(int x, int y, int id, const char* name, SOCKET socket);
	void Login(int x, int y, int hp, int level, int exp, int power);
public:
	// 네트워크
	void do_recv();
	void do_send(void* packet);
	void send_login_info_packet(OBJECT_VISUAL visual);
	void send_login_fail_packet();
	void send_add_player_packet(Object& c_id, char c_visual);
	void send_move_packet(Object& other, char dir);
	void send_remove_player_packet(int c_id);
	void send_exp_change_packet();
	void send_hp_change_packet(int id, int hp);
	void send_level_change_packet(int id, int level, int exp);
	void send_respawn_packet();

public:
	void GetRefViewList(std::unordered_set<int>& view);
	OVER_EXP* GetOverEXP() { return &_recv_over; }
	SOCKET GetSocket() { return _socket; }
	void SetName(const char* name) { strcpy_s(_name, NAME_SIZE, name); }
};

