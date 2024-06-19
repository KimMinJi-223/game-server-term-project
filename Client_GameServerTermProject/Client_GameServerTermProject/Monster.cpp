#include "pch.h"
#include "Monster.h"
#include "ResourceManager.h"
#include "TimeManager.h"
#include "SceneManager.h"
#include "DevScene.h"
Monster::Monster(int visual)
{
	switch (visual)
	{
	case VI_MONSTER_1:
		_flipbookMove[DIR_UP] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_SnakeMove1");
		break;
	case VI_MONSTER_2:
		_flipbookMove[DIR_UP] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_SnakeMove2");
		break;
	case VI_MONSTER_3:
		_flipbookMove[DIR_UP] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_SnakeMove3");
		break;
	case VI_MONSTER_4:
		_flipbookMove[DIR_UP] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_SnakeMove4");
		break;
	default:
		break;
	}

	SetFlipbook(_flipbookMove[DIR_UP]);
}

void Monster::SetDir(Dir dir)
{
	_dir = dir;
}

void Monster::SetState(State state)
{
	if (_state == state)
		return;

	_state = state;
}


void Monster::Tick()
{
	Super::Tick();
	float deltaTime = GET_SINGLE(TimeManager)->GetDeltaTime();

	SetFlipbook(_flipbookMove[0]);

	Vector dir = (_destPos - _pos);
	if (dir.Length() < 2.0f || dir.Length() > 31.f)
	{
		SetState(State::Idle);
		_pos = _destPos;
	}
	else {


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

void Monster::Render(HDC hdc)
{

	Super::Render(hdc);

}

void Monster::SetCellPos(VectorInt cellPos, bool teleport)
{
	_cellPos = cellPos;

	DevScene* scene = dynamic_cast<DevScene*>(GET_SINGLE(SceneManager)->GetCurrentScene());
	if (scene == nullptr)
		return;

	_destPos = scene->ConvertPos(cellPos);

	if (teleport)
		_pos = _destPos;
}