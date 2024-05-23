#include "pch.h"
#include "InputManager.h"

void InputManager::Init(HWND hwnd)
{
	_hwnd = hwnd;
	_states.resize(KEY_TYPE_COUNT, KeyState::None);
}

void InputManager::Update()
{
	// Ű�� �� ��ŭ �޾��� ������ ����
	BYTE asciiKeys[KEY_TYPE_COUNT] = {};

	if (::GetKeyboardState(asciiKeys) == false)
		return;

	// asciiKeys�� ���¸� �����Դ�. 
	// ���°��� �־��ִ�.

	// Ű����
	for (uint32 key = 0; key < KEY_TYPE_COUNT; ++key)
	{
		if (asciiKeys[key] & 0x80)
		{
			KeyState& state = _states[key];

			// ���� �����ӿ� Ű�� ���� ���¶�� PRESS
			if (state == KeyState::Press || state == KeyState::Down)
				state = KeyState::Press;
			else
				state = KeyState::Down;
		}
		else
		{
			KeyState& state = _states[key];

			// ���� �����ӿ� Ű�� ���� ���¶�� UP
			if (state == KeyState::Press || state == KeyState::Down)
				state = KeyState::Up;
			else
				state = KeyState::None;
		}
	}

	// ���콺
	// Ŀ���� ��ǥ�� �˾ƿ´�.
	::GetCursorPos(&_mousePos);
	::ScreenToClient(_hwnd, &_mousePos);
}
