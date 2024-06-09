#include "pch.h"
#include "Monster.h"
#include "ResourceManager.h"

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
	
	//_flipbookMove[DIR_DOWN] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_SnakeMoveDown");
	//_flipbookMove[DIR_LEFT] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_SnakeMoveLeft");
	//_flipbookMove[DIR_RIGHT] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_SnakeMoveRight");
	SetFlipbook(_flipbookMove[DIR_UP]);
}

void Monster::Render(HDC hdc)
{
	Super::Render(hdc);
}

void Monster::Tick()
{
	Super::Tick();
}
