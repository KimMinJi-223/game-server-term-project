#pragma once
#include "..\..\Server_GameServerTermProject\Server_GameServerTermProject\protocol.h"
#include "FlipbookActor.h"

class Flipbook;
class BoxCollider;

enum class PlayerState
{
	Idle,
	Move,
	Skill,
};

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

	void SetState(PlayerState state);
	void SetDir(Dir dir);

	void UpdateAnimation();

	bool HasReachedDest();
	bool CanGo(VectorInt cellPos);
	void SetCellPos(VectorInt cellPos, bool teleport = false);

	int id;
	bool _keyPressed = false;

private:
	Flipbook* _flipbookIdle[4] = {};
	Flipbook* _flipbookMove[4] = {};
	Flipbook* _flipbookAttack[4] = {};

	VectorInt _cellPos = {};
	Vector _speed = {};
	Dir _dir = DIR_DOWN;
	PlayerState _state = PlayerState::Idle;

	int _exp;
	int _level;
public:

};

