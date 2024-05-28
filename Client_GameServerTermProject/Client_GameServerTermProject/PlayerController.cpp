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
	_keyPressed = false;
	player = GET_SINGLE(SceneManager)->GetInstance()->GetCurrentScene()->avatar;
	socket = GET_SINGLE(NetworkManager)->GetInstance()->socket;
}

void PlayerController::Update()
{
	

	if (GET_SINGLE(InputManager)->GetButtonDown(KeyType::W))
	{
		CS_MOVE_PACKET packet;
		packet.size = sizeof(CS_MOVE_PACKET);
		packet.type = static_cast<char>(CS_PACKET_ID::CS_MOVE);
		packet.direction = DIR_UP;
		// Send�ϱ�
		::send(socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0);
	}
	else if (GET_SINGLE(InputManager)->GetButtonDown(KeyType::S))
	{
		CS_MOVE_PACKET packet;
		packet.size = sizeof(CS_MOVE_PACKET);
		packet.type = static_cast<char>(CS_PACKET_ID::CS_MOVE);
		packet.direction = DIR_DOWN;
		// Send�ϱ�
		::send(socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0);
	}
	else if (GET_SINGLE(InputManager)->GetButtonDown(KeyType::A))
	{
		CS_MOVE_PACKET packet;
		packet.size = sizeof(CS_MOVE_PACKET);
		packet.type = static_cast<char>(CS_PACKET_ID::CS_MOVE);
		packet.direction = DIR_LEFT;
		// Send�ϱ�
		::send(socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0);
	}
	else if (GET_SINGLE(InputManager)->GetButtonDown(KeyType::D))
	{
		CS_MOVE_PACKET packet;
		packet.size = sizeof(CS_MOVE_PACKET);
		packet.type = static_cast<char>(CS_PACKET_ID::CS_MOVE);
		packet.direction = DIR_RIGHT;
		// Send�ϱ�
		::send(socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0);
	}
	else if (GET_SINGLE(InputManager)->GetButtonDown(KeyType::Q))
	{
		// Send�ϱ�
	}
	else
	{
		if (_keyPressed == false)
			return; // �̹� ��������
		// Ű �ȴ���
		_keyPressed = false;
		CS_MOVE_STOP_PACKET packet;
		packet.size = sizeof(CS_MOVE_STOP_PACKET);
		packet.type = static_cast<char>(CS_PACKET_ID::CS_MOVE_STOP);
		// Send�ϱ�
	}
}
