#pragma once

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
	bool connectOk = false;
	bool loginOk = false;

public:
	void Init(HWND hwnd, const char* ip);
	void SendLogin(const char* name);
	void Update();

	void ProcessPacket(char* p);
	void process_data(size_t io_byte);
	void SendChat(const char* message);
};

