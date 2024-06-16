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
	void Init(int x, int y, int id, const char* name, SOCKET socket);
	void Login(int x, int y, const char* name, int hp, int level, int exp);
public:
	// ��Ʈ��ũ
	void do_recv();
	void do_send(void* packet);
	void send_login_info_packet(OBJECT_VISUAL visual);
	void send_add_player_packet(Object& c_id, char c_visual);
	void send_move_packet(Object& other, char dir);
	void send_remove_player_packet(int c_id);
	void send_exp_change_packet();
	void send_hp_change_packet(int id, int hp);
	void send_level_change_packet(int id, int level, int exp);

public:
	void GetRefViewList(std::unordered_set<int>& view);
	OVER_EXP* GetOverEXP() { return &_recv_over; }
	SOCKET GetSocket() { return _socket; }
};

