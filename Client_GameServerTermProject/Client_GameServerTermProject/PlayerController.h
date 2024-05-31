#pragma once
#include "Player.h"

class PlayerController
{
private:
	Player* player;
	SOCKET socket;
	float _keyPressTime;
public:
	PlayerController();
	~PlayerController();

public:
	void Init();
	void Update();
};

