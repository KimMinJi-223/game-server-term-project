#pragma once

enum class KeyType
{
	LeftMouse = VK_LBUTTON,
	RightMouse = VK_RBUTTON,

	Up = VK_UP,
	Down = VK_DOWN,
	Left = VK_LEFT,
	Right = VK_RIGHT,
	SpaceBar = VK_SPACE,

	W = 'W',
	A = 'A',
	S = 'S',
	D = 'D',

	Q = 'Q',
	E = 'E'
};

enum class KeyState
{
	None,
	Press,
	Down,
	Up,

	End
};

enum
{
	KEY_TYPE_COUNT = static_cast<int32>(UINT8_MAX) + 1,
	KEY_STATE_COUNT = static_cast<int32>(KeyState::End)
};

class InputManager
{
	DECLARE_SINGLE(InputManager);

public:
	// 지금 당장은 hwnd가 필요가 없지만
	// 나중에 현재 창이 선택됐는지 확인할 필요가 있을 수도 있다. 
	void Init(HWND hwnd);
	// 매 프레임마다 현재 인풋의 상태를 보는 함수
	void Update();

	// 버튼을 누르고 있을때 
	bool GetButton(KeyType key) { return GetState(key) == KeyState::Press; }

	// 맨 처음 눌렀을 때
	bool GetButtonDown(KeyType key) { return GetState(key) == KeyState::Down; }

	// 맨 처음 눌렀다가 땔 때
	bool GetButtonUp(KeyType key) { return GetState(key) == KeyState::Up; }

	POINT GetMoustPos() { return _mousePos; }
	HWND _hwnd;

private:
	KeyState GetState(KeyType key) { return _states[static_cast<uint8>(key)]; }

private:
	vector<KeyState> _states;
	POINT _mousePos;
	
};