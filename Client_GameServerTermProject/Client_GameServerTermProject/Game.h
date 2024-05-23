#pragma once

class Game
{
public:
	Game();
	~Game();

public:
	// �ʱ�ȭ�ϴ� �Լ�
	void Init(HWND hwnd, HINSTANCE hinst);
	// �� �����Ӹ��� ������Ʈ�ϴ� �Լ�
	void Update();
	// �׸� �׸��� ���� ó���ϴ� �Լ�
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

