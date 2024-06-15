#pragma once

#include "FlipbookActor.h"
#include "Defines.h"

class Flipbook;
class BoxCollider;
class Player : public FlipbookActor
{
	using Super = FlipbookActor;

public:
	Player();
	virtual ~Player() override;

	virtual void BeginPlay() override;
	virtual void Tick() override;
	virtual void Render(HDC hdc) override;

	virtual void TickIdle();
	virtual void TickMove();
	virtual void TickSkill();

	void SetState(State state);
	void SetDir(Dir dir);

	void UpdateAnimation();

	bool CanGo(VectorInt cellPos);
	void SetCellPos(VectorInt cellPos, bool teleport = false);

	void SetEXP(int exp) { _exp = exp; }
	int GetEXP() { return _exp; }

	int _id;
	bool _keyPressed = false;

private:
	Flipbook* _flipbookIdle[4] = {};
	Flipbook* _flipbookMove[4] = {};
	Flipbook* _flipbookAttack[4] = {};

	VectorInt _cellPos = {};
	Vector _speed = {};
	Dir _dir = DIR_DOWN;
	State _state = State::Idle;

	int _exp;

public:

};

