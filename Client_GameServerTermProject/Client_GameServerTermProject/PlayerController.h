#pragma once
#include "Player.h"

class PlayerController
{
private:
	Player* player;
	SOCKET socket;

public:
	PlayerController();
	~PlayerController();

public:
	void Init();
	void Update();


};

