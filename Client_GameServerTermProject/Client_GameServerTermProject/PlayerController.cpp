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
}

void PlayerController::Update()
{
	
	if (player->_keyPressed) {
		return;
	}

	if (GET_SINGLE(InputManager)->GetButton(KeyType::W))
	{
		CS_MOVE_PACKET packet;
		packet.size = sizeof(CS_MOVE_PACKET);
		packet.type = static_cast<char>(CS_PACKET_ID::CS_MOVE);
		packet.direction = DIR_UP;
		// Send�ϱ�
		player->_keyPressed = true;
		::send(socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0);
	}
	else if (GET_SINGLE(InputManager)->GetButton(KeyType::S))
	{
		CS_MOVE_PACKET packet;
		packet.size = sizeof(CS_MOVE_PACKET);
		packet.type = static_cast<char>(CS_PACKET_ID::CS_MOVE);
		packet.direction = DIR_DOWN;
		// Send�ϱ�
		player->_keyPressed = true;
		::send(socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0);

	}
	else if (GET_SINGLE(InputManager)->GetButton(KeyType::A))
	{
		CS_MOVE_PACKET packet;
		packet.size = sizeof(CS_MOVE_PACKET);
		packet.type = static_cast<char>(CS_PACKET_ID::CS_MOVE);
		packet.direction = DIR_LEFT;
		// Send�ϱ�
		player->_keyPressed = true;
		::send(socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0);

	}
	else if (GET_SINGLE(InputManager)->GetButton(KeyType::D))
	{
		CS_MOVE_PACKET packet;
		packet.size = sizeof(CS_MOVE_PACKET);
		packet.type = static_cast<char>(CS_PACKET_ID::CS_MOVE);
		packet.direction = DIR_RIGHT;
		// Send�ϱ�
		player->_keyPressed = true;
		::send(socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0);

	}
	else if (GET_SINGLE(InputManager)->GetButton(KeyType::Q))
	{
		// Send�ϱ�
	}
	else
	{
		if (player->_keyPressed == false)
			return; // �̹� ��������
		// Ű �ȴ���
		player->_keyPressed = false;
		CS_MOVE_STOP_PACKET packet;
		packet.size = sizeof(CS_MOVE_STOP_PACKET);
		packet.type = static_cast<char>(CS_PACKET_ID::CS_MOVE_STOP);
		// Send�ϱ�
	}
	
}
