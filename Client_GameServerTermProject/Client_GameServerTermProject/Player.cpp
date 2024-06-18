#include "pch.h"
#include "Player.h"
#include "InputManager.h"
#include "TimeManager.h"
#include "ResourceManager.h"
#include "Flipbook.h"
#include "CameraComponent.h"
#include "SceneManager.h"
#include "DevScene.h"
#include "NetworkManager.h"

Player::Player()
{
	int a = sizeof(Player);
	_hp = 50;
	_exp = 100;
	_level = 1;
	//_speed.x = 30;
	//_speed.y = 30;

	_speed.x = 250;
	_speed.y = 250;


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

	SetState(State::Move);
	SetState(State::Idle);

	SetCellPos({ 5, 5 }, true);
}

void Player::Tick()
{
	Super::Tick();

	switch (_state)
	{
	case State::Idle:
		TickIdle();
		break;
	case State::Move:
		TickMove();
		break;
	case State::Skill:
		TickSkill();
		break;
	}
}

void Player::Render(HDC hdc)
{
	Super::Render(hdc);

	if (GET_SINGLE(NetworkManager)->GetInstance()->myId == _id) {
	::Rectangle(hdc, 50, 0, 150, 15);

		// exp
		std::wstring str = std::format(L"EXP : {0}", _exp);
		::TextOut(hdc, 50, 0, str.c_str(), static_cast<int32>(str.size()));

		// 체력
		str = std::format(L"HP : {0}", _hp);
		::TextOut(hdc, 50, 15, str.c_str(), static_cast<int32>(str.size()));

		// 레벨
		str = std::format(L"LEVEL : {0}", _level);
		::TextOut(hdc, 50, 30, str.c_str(), static_cast<int32>(str.size()));

		POINT mousePos = GET_SINGLE(InputManager)->GetMoustPos();

		str = std::format(L"Pos({0}, {1})", (int)_pos.x / 30, (int)_pos.y / 30);
		::TextOut(hdc, 450, 10, str.c_str(), static_cast<int32>(str.size()));

	}
}

void Player::TickIdle()
{
	float deltaTime = GET_SINGLE(TimeManager)->GetDeltaTime();

	if (_state == State::Idle)
		UpdateAnimation();
}

void Player::TickMove()
{
	float deltaTime = GET_SINGLE(TimeManager)->GetDeltaTime();
	
	Vector dir = (_destPos - _pos);
	if (dir.Length() < 2.0f || dir.Length() > 31.f)
	{
		SetState(State::Idle);
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
		SetState(State::Idle);
}

void Player::SetState(State state)
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
	case State::Idle:
		if (_keyPressed)
			SetFlipbook(_flipbookMove[_dir]);	
		else
			SetFlipbook(_flipbookIdle[_dir]);
		break;
	case State::Move:
		SetFlipbook(_flipbookMove[_dir]);
		break;
	case State::Skill:
		SetFlipbook(_flipbookAttack[_dir]);
		break;
	}
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