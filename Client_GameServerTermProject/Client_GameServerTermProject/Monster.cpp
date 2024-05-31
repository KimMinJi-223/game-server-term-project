#include "pch.h"
#include "Monster.h"
#include "..\..\Server_GameServerTermProject\Server_GameServerTermProject\protocol.h"
#include "ResourceManager.h"

Monster::Monster()
{
	_flipbookMove[DIR_UP] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_SnakeMoveUp");
	_flipbookMove[DIR_DOWN] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_SnakeMoveDown");
	_flipbookMove[DIR_LEFT] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_SnakeMoveLeft");
	_flipbookMove[DIR_RIGHT] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_SnakeMoveRight");
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
