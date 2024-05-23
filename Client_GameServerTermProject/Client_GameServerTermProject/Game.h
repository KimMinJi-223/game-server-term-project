#pragma once

class Game
{
public:
	Game();
	~Game();

public:
	// 초기화하는 함수
	void Init(HWND hwnd, HINSTANCE hinst);
	// 매 프레임마다 업데이트하는 함수
	void Update();
	// 그림 그리는 것을 처리하는 함수
	void Render();

private:
	HWND _hwnd = {};
	HDC _hdc = {};
	HINSTANCE _hinst = {};
private:
	// Double Buffering
	RECT _rect;
	HDC _hdcBack;
	HBITMAP _bmpBack;
};

