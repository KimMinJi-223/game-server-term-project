#pragma once
#include "FlipbookActor.h"

class Monster : public FlipbookActor
{
	using Super = FlipbookActor;
	//UI ui;
public:
	Monster(int visual);

	Flipbook* _flipbookIdle[4] = {};
	Flipbook* _flipbookMove[4] = {};
	Flipbook* _flipbookAttack[4] = {};

	virtual void Render(HDC hdc) override;
	virtual void Tick() override;
};

