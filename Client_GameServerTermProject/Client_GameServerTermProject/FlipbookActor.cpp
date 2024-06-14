#include "pch.h"
#include "FlipbookActor.h"
#include "Flipbook.h"
#include "Sprite.h"
#include "TimeManager.h"
#include "SceneManager.h"
#include "Texture.h"
#include "NetworkManager.h"

FlipbookActor::FlipbookActor()
{
	
}

FlipbookActor::~FlipbookActor()
{
}

void FlipbookActor::BeginPlay()
{
	Super::BeginPlay();
}

void FlipbookActor::Tick()
{
	// 애니메이션이 일직선으로 있다고 가정

	Super::Tick();

	if (_flipbook == nullptr)
		return;

	const FlipbookInfo& info = _flipbook->GetInfo();
	if (info.loop == false && _idx == info.end)
		return;

	float deltaTime = GET_SINGLE(TimeManager)->GetDeltaTime();

	_sumTime += deltaTime;

	// 플립북의 그림이 몇개가 있는지
	int32 frameConut = (info.end - info.start + 1);
	// duration안에 현재 플립북이 다 실행되어야 한다. 
	float delta = info.duration / frameConut;

	if (_sumTime >= delta)
	{
		_sumTime = 0.f;
		_idx = (_idx + 1) % frameConut;
	}
}

void FlipbookActor::Render(HDC hdc)
{
	Super::Render(hdc);
	
	if (_flipbook == nullptr)
		return;

	const FlipbookInfo& info = _flipbook->GetInfo();

	Vector cameraPos = GET_SINGLE(SceneManager)->GetCameraPos();

 	::TransparentBlt(hdc,
		(int32)_pos.x - info.size.x / 2 - ((int32)cameraPos.x - GWinSizeX / 2), 
		(int32)_pos.y - info.size.y / 2 - ((int32)cameraPos.y - GWinSizeY / 2),
		info.size.x, info.size.y,
		info.texture->GetDC(),
		(info.start + _idx) * info.size.x,
		info.line * info.size.y,
		info.size.x, info.size.y,
		info.texture->GetTransparent()); 

	HFONT hFont = CreateFont(
		10, 0, 0, 0,                  
		FW_NORMAL, FALSE, FALSE, FALSE,              
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");     

	HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
	// 이름
	wchar_t name[NAME_SIZE / 2];
	MultiByteToWideChar(CP_ACP, 0, GetName(), -1, name, NAME_SIZE / 2);
	wstring str = std::format(L"{0}", name);
	::TextOut(hdc, (int32)_pos.x - ((int32)cameraPos.x - GWinSizeX / 2) - str.size() * 2,
		(int32)_pos.y - ((int32)cameraPos.y - GWinSizeY / 2) - 40,
		str.c_str(), static_cast<int32>(str.size()));
	str = std::format(L"L:{0}, HP:{1}", _level, _hp);
	::TextOut(hdc, (int32)_pos.x - ((int32)cameraPos.x - GWinSizeX / 2) - str.size() * 2,
		(int32)_pos.y - ((int32)cameraPos.y - GWinSizeY / 2) - 30,
		str.c_str(), static_cast<int32>(str.size()));

	SelectObject(hdc, hOldFont);

	DeleteObject(hFont);

}

void FlipbookActor::SetFlipbook(Flipbook* flipbook)
{
	if (flipbook && _flipbook == flipbook)
		return;

	_flipbook = flipbook;
	//Reset();
}

void FlipbookActor::Reset()
{
	_sumTime = 0.f;
	_idx = _flipbook->GetInfo().start;
}