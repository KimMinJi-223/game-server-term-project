#pragma once
#include "..\..\Server_GameServerTermProject\Server_GameServerTermProject\protocol.h"
class Player;

class NetworkManager
{
public:
	DECLARE_SINGLE(NetworkManager);
	SOCKET socket;
	char recvBuffer[BUF_SIZE];
	int prev_remain;
	int myId;
	Player* avatar;
	HWND _hwnd;
public:
	void Init(HWND hwnd);
	void Update();

	void ProcessPacket(char* p);
	void process_data(size_t io_byte);
	void SendChat(const char* message);
};

