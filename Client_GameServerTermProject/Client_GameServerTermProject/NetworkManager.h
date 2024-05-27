#pragma once
class NetworkManager
{
public:
	DECLARE_SINGLE(NetworkManager);
	int myId;
	SOCKET socket;

public:
	void Init();
	void Update();

	void ProcessPacket(char* ptr);
};

