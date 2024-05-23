#include "pch.h"
#include "InputManager.h"

void InputManager::Init(HWND hwnd)
{
	_hwnd = hwnd;
	_states.resize(KEY_TYPE_COUNT, KeyState::None);
}

void InputManager::Update()
{
	// 키의 수 만큼 받아줄 공간을 마련
	BYTE asciiKeys[KEY_TYPE_COUNT] = {};

	if (::GetKeyboardState(asciiKeys) == false)
		return;

	// asciiKeys에 상태를 가져왔다. 
	// 상태값을 넣어주다.

	// 키보드
	for (uint32 key = 0; key < KEY_TYPE_COUNT; ++key)
	{
		if (asciiKeys[key] & 0x80)
		{
			KeyState& state = _states[key];

			// 이전 프레임에 키를 누른 상태라면 PRESS
			if (state == KeyState::Press || state == KeyState::Down)
				state = KeyState::Press;
			else
				state = KeyState::Down;
		}
		else
		{
			KeyState& state = _states[key];

			// 이전 프레임에 키를 누른 상태라면 UP
			if (state == KeyState::Press || state == KeyState::Down)
				state = KeyState::Up;
			else
				state = KeyState::None;
		}
	}

	// 마우스
	// 커서의 좌표를 알아온다.
	::GetCursorPos(&_mousePos);
	::ScreenToClient(_hwnd, &_mousePos);
}
