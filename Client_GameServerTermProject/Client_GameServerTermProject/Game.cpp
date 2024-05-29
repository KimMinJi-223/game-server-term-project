#include "pch.h"
#include "Game.h"
#include "TimeManager.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "SoundManager.h"
#include "NetworkManager.h"
#include "PlayerController.h"
#include "Scene.h"

Game::Game()
{

}

Game::~Game()
{
	GET_SINGLE(SceneManager)->Clear();
	GET_SINGLE(ResourceManager)->Clear();
	
	_CrtDumpMemoryLeaks();
}
void Game::Init(HWND hwnd, HINSTANCE hinst)
{
	_hwnd = hwnd;				// 윈도우 번호
	_hdc = ::GetDC(hwnd);		// 우리가 그릴 도화지 번호

	// 600 600 그 영역을 말하는 것이다. 
	::GetClientRect(hwnd, &_rect);
	// _hdc와 호환이 되는 DC를 생성한다.
	_hdcBack = ::CreateCompatibleDC(_hdc);
	// _hdc와 호환되는 비트맵 생성
	_bmpBack = ::CreateCompatibleBitmap(_hdc, _rect.right, _rect.bottom);
	// DC와 BMP를 연결
	HBITMAP prev = (HBITMAP)::SelectObject(_hdcBack, _bmpBack);
	::DeleteObject(prev);


	GET_SINGLE(TimeManager)->Init();
	GET_SINGLE(InputManager)->Init(hwnd);
	GET_SINGLE(SceneManager)->Init();
	GET_SINGLE(ResourceManager)->Init(hwnd, fs::path(L"../Resources"));
	GET_SINGLE(SoundManager)->Init(hwnd);

	// 초기 씬 설정
	GET_SINGLE(SceneManager)->ChangeScene(SceneType::DevScene);

	GET_SINGLE(NetworkManager)->Init(_hwnd);
	GET_SINGLE(SceneManager)->GetCurrentScene()->GetPlayerController()->Init();
}

void Game::Update()
{
	// 통신 
	GET_SINGLE(NetworkManager)->Update();
	// 현재 시간 계산
	GET_SINGLE(TimeManager)->Update();
	// 입력의 상태
	GET_SINGLE(InputManager)->Update();
	// 씬
	GET_SINGLE(SceneManager)->Update();
}

void Game::Render()
{
	uint32 fps = GET_SINGLE(TimeManager)->GetFps();
	float deltaTime = GET_SINGLE(TimeManager)->GetDeltaTime();

	GET_SINGLE(SceneManager)->Render(_hdcBack);

	{
		POINT mousePos = GET_SINGLE(InputManager)->GetMoustPos();
		wstring str = std::format(L"Mouse({0}, {1})", mousePos.x, mousePos.y);
		::TextOut(_hdcBack, 500, 10, str.c_str(), static_cast<int32>(str.size()));
	}

	{
		wstring str = std::format(L"FPS[{0}], DT[{1}] ms", fps, static_cast<int32>(deltaTime * 1000));
		::TextOut(_hdcBack, 650, 10, str.c_str(), static_cast<int32>(str.size()));
	}

	// 채팅창 제외하고 그린다. 
	BitBlt(_hdc, 0, 0, 600, 400, _hdcBack, 0, 0, SRCCOPY);
	BitBlt(_hdc, 300, 0, _rect.right - (300), _rect.bottom, _hdcBack, 300, 0, SRCCOPY);

	// back도화지를 흰색으로 지운다.
	PatBlt(_hdcBack, 0, 0, _rect.right, _rect.bottom, WHITENESS);
}
