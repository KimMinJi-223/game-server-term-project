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
	int		_prev_remain;

public:
	// 스트레스 테스트
	int		_last_move_time;

public:
	void Init(int x, int y, int id, const char* name, SOCKET socket);

public:
	// 네트워크
	void do_recv();
	void do_send(void* packet);
	void send_login_info_packet(OBJECT_VISUAL visual);
	void send_add_player_packet(Object& c_id, char c_visual);

public:
	int GetPrevRemain() { return _prev_remain; }
	void SetPrevRemain(int prev) { _prev_remain = prev; }
	void GetRefViewList(std::unordered_set<int>& view);
	void send_move_packet(Object& other);
	void send_remove_player_packet(int c_id);
};

