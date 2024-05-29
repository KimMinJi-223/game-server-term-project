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
#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

class Server
{
private:
	SOCKET _listen_socket;
	SOCKET _client_socket;
	OVER_EXP _accept_over;
	HANDLE _hiocp;
	std::array<Object*, MAX_USER + NUM_NPC> objects;
	std::array<Sector, SECTOR_COUNT> sectors;
	Timer _timerQueue; 

public:
	void Init();
	bool can_see(int a, int b);

public:
	void process_packet(int id, char* packet);
	void WorkerThread();
	void disconnect(int key);
	void BroadCastChat(int id, char* packet);
public:
	int get_new_client_id();

};

