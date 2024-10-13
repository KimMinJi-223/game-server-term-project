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
	void DoRecv();
	void DoSend(void* packet);
	void SendLoginInfoPacket(OBJECT_VISUAL visual);
	void SendLoginFailPacket();
	void SendAddPlayerPacket(Object& c_id, char c_visual);
	void SendMovePacket(Object& other, char dir);
	void SendRemovePlayerPacket(int c_id);
	void SendExpChangePacket();
	void SendHpChangePacket(int id, int hp);
	void SendLevelChangePacket(int id, int level, int exp);
	void SendRespawnPacket();

public:
	void GetRefViewList(std::unordered_set<int>& view);
	void ClearViewList();
	OVER_EXP* GetOverEXP() { return &_recv_over; }
	SOCKET GetSocket() { return _socket; }
	void SetName(const char* name) { strcpy_s(_name, NAME_SIZE, name); }
};

