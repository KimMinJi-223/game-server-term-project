#include "pch.h"
#include "PlayerController.h"
#include "SceneManager.h"
#include "Scene.h"
#include "InputManager.h"
#include "TimeManager.h"
#include "NetworkManager.h"

PlayerController::PlayerController()
{
}

PlayerController::~PlayerController()
{
}

void PlayerController::Init()
{
	player = GET_SINGLE(SceneManager)->GetInstance()->GetCurrentScene()->avatar;
	socket = GET_SINGLE(NetworkManager)->GetInstance()->socket;
	_keyPressTime = 1.f;
}

void PlayerController::Update()
{
	bool isPress = false;
	CS_MOVE_PACKET packet;
	if (GET_SINGLE(InputManager)->GetButton(KeyType::Up))
	{
		packet.size = sizeof(CS_MOVE_PACKET);
		packet.type = CS_MOVE;
		packet.direction = DIR_UP;
		// Send하기
		isPress = true;
		player->_keyPressed = true;
	}
	else if (GET_SINGLE(InputManager)->GetButton(KeyType::Down))
	{
		packet.size = sizeof(CS_MOVE_PACKET);
		packet.type = CS_MOVE;
		packet.direction = DIR_DOWN;
		// Send하기
		isPress = true;

		player->_keyPressed = true;

	}
	else if (GET_SINGLE(InputManager)->GetButton(KeyType::Left))
	{
		packet.size = sizeof(CS_MOVE_PACKET);
		packet.type = CS_MOVE;
		packet.direction = DIR_LEFT;
		// Send하기
		isPress = true;

		player->_keyPressed = true;

	}
	else if (GET_SINGLE(InputManager)->GetButton(KeyType::Right))
	{
		packet.size = sizeof(CS_MOVE_PACKET);
		packet.type = CS_MOVE;
		packet.direction = DIR_RIGHT;
		// Send하기
		isPress = true;

		player->_keyPressed = true;
		

	}
	else if (GET_SINGLE(InputManager)->GetButtonDown(KeyType::S))
	{
		CS_ATTACK_PACKET packet;
		packet.size = sizeof(CS_ATTACK_PACKET);
		packet.type = CS_ATTACK;
		player->SetState(State::Skill);
		player->Reset();
		::send(socket, reinterpret_cast<char*>(&packet), sizeof(CS_ATTACK_PACKET), 0);
	}
	else if (GET_SINGLE(InputManager)->GetButtonDown(KeyType::A))
	{
		CS_A_ATTACK_PACKET packet;
		packet.size = sizeof(CS_A_ATTACK_PACKET);
		packet.type = CS_A_SKILL;
		player->SetState(State::Skill);
		player->Reset();
		::send(socket, reinterpret_cast<char*>(&packet), sizeof(CS_A_ATTACK_PACKET), 0);
	}
	else {
		// 버튼 뗀경우 
		player->_keyPressed = false;
	}
	// 1초에 한번 서버에 입력키를 보낸다. (안 눌렀으면 안보냄)
	float deltaTime = GET_SINGLE(TimeManager)->GetDeltaTime();
	if (_keyPressTime <= 0.f) {
		player->_keyPressed = false;
		if (isPress) {
			::send(socket, reinterpret_cast<char*>(&packet), sizeof(CS_MOVE_PACKET), 0);
			player->_keyPressed = true;
			//_keyPressTime = 1.0f;
			_keyPressTime = 0.1f;
			return;
		}
	}
	_keyPressTime -= deltaTime;

}
