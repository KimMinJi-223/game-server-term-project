#pragma once
#include "FlipbookActor.h"
#include "Defines.h"
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
	void SetCellPos(VectorInt cellPos, bool teleport = false);
	void SetDir(Dir dir);
	void SetState(State state);

private:
	VectorInt _cellPos = {};
	Dir _dir = DIR_DOWN;
	Vector _speed = {50, 50};
	State _state = State::Idle;
};

