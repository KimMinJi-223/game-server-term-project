#include "pch.h"
#include "Player.h"
#include "InputManager.h"
#include "TimeManager.h"
#include "ResourceManager.h"
#include "Flipbook.h"
#include "CameraComponent.h"
//#include "BoxCollider.h"
#include "SceneManager.h"
#include "DevScene.h"
#include <iostream>
Player::Player()
{
	int a = sizeof(Player);
	_hp = 50;
	_exp = 100;
	_level = 1;
	_speed.x = 30;
	_speed.y = 30;

	_flipbookIdle[DIR_UP] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_IdleUp");
	_flipbookIdle[DIR_DOWN] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_IdleDown");
	_flipbookIdle[DIR_LEFT] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_IdleLeft");
	_flipbookIdle[DIR_RIGHT] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_IdleRight");

	_flipbookMove[DIR_UP] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_MoveUp");
	_flipbookMove[DIR_DOWN] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_MoveDown");
	_flipbookMove[DIR_LEFT] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_MoveLeft");
	_flipbookMove[DIR_RIGHT] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_MoveRight");

	_flipbookAttack[DIR_UP] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_AttackUp");
	_flipbookAttack[DIR_DOWN] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_AttackDown");
	_flipbookAttack[DIR_LEFT] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_AttackLeft");
	_flipbookAttack[DIR_RIGHT] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_AttackRight");


}

Player::~Player()
{
}

void Player::BeginPlay()
{
	Super::BeginPlay();

	SetState(PlayerState::Move);
	SetState(PlayerState::Idle);

	SetCellPos({ 5, 5 }, true);
}

void Player::Tick()
{
	Super::Tick();

	switch (_state)
	{
	case PlayerState::Idle:
		TickIdle();
		break;
	case PlayerState::Move:
		TickMove();
		break;
	case PlayerState::Skill:
		TickSkill();
		break;
	}
}

void Player::Render(HDC hdc)
{
	Super::Render(hdc);

	::Rectangle(hdc, 50, 0, 150, 15);
	::Rectangle(hdc, 50, 15, 150, 30);

	HBRUSH hBrushRed = CreateSolidBrush(RGB(255, 0, 0));
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrushRed);
	::Rectangle(hdc, 50, 0, _hp + 50, 15);
	SelectObject(hdc, hOldBrush);
	DeleteObject(hBrushRed);

	HBRUSH hBrushGreen = CreateSolidBrush(RGB(0, 255, 0));
	hOldBrush = (HBRUSH)SelectObject(hdc, hBrushGreen);
	::Rectangle(hdc, 50, 15, _exp + 50, 30);
	SelectObject(hdc, hOldBrush);
	DeleteObject(hBrushGreen);

	// ·¹º§
	wstring str = std::format(L"LEVEL : {0}", _level);
	::TextOut(hdc, 50, 30, str.c_str(), static_cast<int32>(str.size()));
}

void Player::TickIdle()
{
	float deltaTime = GET_SINGLE(TimeManager)->GetDeltaTime();

	if (_state == PlayerState::Idle)
		UpdateAnimation();
}

void Player::TickMove()
{
	float deltaTime = GET_SINGLE(TimeManager)->GetDeltaTime();
	
	Vector dir = (_destPos - _pos);
	if (dir.Length() < 2.0f || dir.Length() > 31.f)
	{
		SetState(PlayerState::Idle);
		_pos = _destPos;
		//_keyPressed = false;
	}
	else
	{
		switch (_dir)
		{
		case DIR_UP:
			_pos.y -= _speed.y * deltaTime;
			break;
		case DIR_DOWN:
			_pos.y += _speed.y * deltaTime;
			break;
		case DIR_LEFT:
			_pos.x -= _speed.x * deltaTime;
			break;
		case DIR_RIGHT:
			_pos.x += _speed.x * deltaTime;
			break;
		}
	}
}
void Player::TickSkill()
{
	float deltaTime = GET_SINGLE(TimeManager)->GetDeltaTime();

	if (_idx == 7)
		SetState(PlayerState::Idle);
}

void Player::SetState(PlayerState state)
{
	if (_state == state)
		return;

	_state = state;
	UpdateAnimation();
}

void Player::SetDir(Dir dir)
{
	_dir = dir;
	UpdateAnimation();
}

void Player::UpdateAnimation()
{
	switch (_state)
	{
	case PlayerState::Idle:
		if (_keyPressed)
			SetFlipbook(_flipbookMove[_dir]);	
		else
			SetFlipbook(_flipbookIdle[_dir]);
		break;
	case PlayerState::Move:
		SetFlipbook(_flipbookMove[_dir]);
		break;
	case PlayerState::Skill:
		SetFlipbook(_flipbookAttack[_dir]);
		break;
	}
}

bool Player::HasReachedDest()
{
	Vector dir = (_destPos - _pos);
	return (dir.Length() < 10.f);
}

bool Player::CanGo(VectorInt cellPos)
{
	DevScene* scene = dynamic_cast<DevScene*>(GET_SINGLE(SceneManager)->GetCurrentScene());
	if (scene == nullptr)
		return false;

	return scene->CanGo(cellPos);
}

void Player::SetCellPos(VectorInt cellPos, bool teleport)
{

	_cellPos = cellPos;
	
	DevScene* scene = dynamic_cast<DevScene*>(GET_SINGLE(SceneManager)->GetCurrentScene());
	if (scene == nullptr)
		return;

	_destPos = scene->ConvertPos(cellPos);

	if (teleport)
		_pos = _destPos;
}