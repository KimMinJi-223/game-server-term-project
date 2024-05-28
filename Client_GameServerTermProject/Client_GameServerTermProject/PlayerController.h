#pragma once
#include "Player.h"

class PlayerController
{
private:
	Player* player;
	bool _keyPressed;
	SOCKET socket;

public:
	PlayerController();
	~PlayerController();

public:
	void Init();
	void Update();


};

