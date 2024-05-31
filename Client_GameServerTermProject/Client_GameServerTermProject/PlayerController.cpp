#include "..\..\Server_GameServerTermProject\Server_GameServerTermProject\protocol.h"
#include "Defines.h"
#include "PlayerController.h"
#include "SceneManager.h"
#include "Scene.h"
#include "InputManager.h"
#include "TimeManager.h"
#include "Enums.h"
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
	if (GET_SINGLE(InputManager)->GetButton(KeyType::W))
	{
		packet.size = sizeof(CS_MOVE_PACKET);
		packet.type = static_cast<char>(CS_PACKET_ID::CS_MOVE);
		packet.direction = DIR_UP;
		// Send하기
		isPress = true;
		player->_keyPressed = true;
	}
	else if (GET_SINGLE(InputManager)->GetButton(KeyType::S))
	{
		packet.size = sizeof(CS_MOVE_PACKET);
		packet.type = static_cast<char>(CS_PACKET_ID::CS_MOVE);
		packet.direction = DIR_DOWN;
		// Send하기
		isPress = true;

		player->_keyPressed = true;

	}
	else if (GET_SINGLE(InputManager)->GetButton(KeyType::A))
	{
		packet.size = sizeof(CS_MOVE_PACKET);
		packet.type = static_cast<char>(CS_PACKET_ID::CS_MOVE);
		packet.direction = DIR_LEFT;
		// Send하기
		isPress = true;

		player->_keyPressed = true;

	}
	else if (GET_SINGLE(InputManager)->GetButton(KeyType::D))
	{
		packet.size = sizeof(CS_MOVE_PACKET);
		packet.type = static_cast<char>(CS_PACKET_ID::CS_MOVE);
		packet.direction = DIR_RIGHT;
		// Send하기
		isPress = true;

		player->_keyPressed = true;
		

	}
	else if (GET_SINGLE(InputManager)->GetButton(KeyType::Q))
	{
		// Send하기
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
			_keyPressTime = 1.0f;
			return;
		}
	}
	_keyPressTime -= deltaTime;

}
