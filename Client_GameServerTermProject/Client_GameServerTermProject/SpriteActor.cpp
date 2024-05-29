#include "pch.h"
#include "SpriteActor.h"
#include "Sprite.h"
#include "SceneManager.h"

SpriteActor::SpriteActor()
{
}

SpriteActor::~SpriteActor()
{
}

void SpriteActor::BeginPlay()
{
	Super::BeginPlay();
}

void SpriteActor::Tick()
{
	Super::Tick();

}

void SpriteActor::Render(HDC hdc)
{
	Super::Render(hdc);

	if (_sprite == nullptr)
		return;

	VectorInt size = _sprite->GetSize();

	Vector cameraPos = GET_SINGLE(SceneManager)->GetCameraPos();

	::TransparentBlt(hdc, 
		(int32)_pos.x - size.x / 2 - ((int32)cameraPos.x - GWinSizeX / 2), 
		(int32)_pos.y - size.y / 2 - ((int32)cameraPos.y - GWinSizeY / 2), 
		size.x, size.y,
		_sprite->GetDC(),
		_sprite->GetPos().x, _sprite->GetPos().y, 
		_sprite->GetSize().x, _sprite->GetSize().y,
		_sprite->GetTransparent());

	// 지금은 배경만 그리고 있는데 TransparentBlt보다 Bitblt가 더 빠르다. 
	//StretchBlt(hdc, 0, 0, size.x/2, size.y/2, _sprite->GetDC(), 0,0, size.x, size.y, SRCCOPY);
	/*::BitBlt(hdc,
		(int32)_pos.x - size.x / 2 - ((int32)cameraPos.x - GWinSizeX / 2),
		(int32)_pos.y - size.y / 2 - ((int32)cameraPos.y - GWinSizeY / 2),
		size.x, size.y,
		_sprite->GetDC(),
		_sprite->GetPos().x, _sprite->GetPos().y,
		SRCCOPY);*/
}
