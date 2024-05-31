#include <WS2tcpip.h>
#include <MSWSock.h>
#include "Player.h"
#include "Monster.h"
#include "NetworkManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Actor.h"
#include <string>

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

void NetworkManager::Init(HWND hwnd)
{
	_hwnd = hwnd;
	avatar = GET_SINGLE(SceneManager)->GetInstance()->GetCurrentScene()->avatar;

	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, 0);

	// 넌블럭킹으로
	unsigned long noblock = 1;
	int nRet = ioctlsocket(socket, FIONBIO, &noblock);

	SOCKADDR_IN addr_s;
	addr_s.sin_family = AF_INET;
	// 엔디안 맞춘다
	addr_s.sin_port = htons(PORT_NUM);
	inet_pton(AF_INET, "127.0.0.1", &addr_s.sin_addr);

	while (true) {
		if (connect(socket, reinterpret_cast<SOCKADDR*>(&addr_s), sizeof(addr_s)) == SOCKET_ERROR) {
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;

			if (::WSAGetLastError() == WSAEISCONN)
				break;
		}
	}
	// CS_LOGIN 해야함
	CS_LOGIN_PACKET p;
	p.size = sizeof(p);
	p.type = static_cast<char>(CS_PACKET_ID::CS_LOGIN);
	strcpy_s(p.name, "hello");
	::send(socket, reinterpret_cast<char*>(&p), sizeof(p), 0);
}

void NetworkManager::Update()
{
	int32 recvLen = ::recv(socket, recvBuffer + prev_remain, sizeof(recvBuffer) - prev_remain, 0);
	if (recvLen == SOCKET_ERROR)
	{
		if (::WSAGetLastError() != WSAEWOULDBLOCK)
			exit(-1);
	}

	if (recvLen > 0)
		process_data(recvLen);
}

void NetworkManager::ProcessPacket(char* p)
{
	switch (p[1])
	{
	case static_cast<int>(SC_PACKET_ID::SC_LOGIN_INFO):
	{
		SC_LOGIN_INFO_PACKET* packet = reinterpret_cast<SC_LOGIN_INFO_PACKET*>(p);
		myId = packet->id;
		VectorInt pos = { packet->x, packet->y };

		avatar->SetCellPos(pos, true);
	}
	break;
	case static_cast<int>(SC_PACKET_ID::SC_ADD_PLAYER):
	{
		SC_ADD_PLAYER_PACKET* my_packet = reinterpret_cast<SC_ADD_PLAYER_PACKET*>(p);
		int id = my_packet->id;

		if (id == myId) {
			avatar->SetCellPos(VectorInt{ my_packet->x, my_packet->y }, true);
			avatar->SetState(PlayerState::Idle);
		}
		else {
			if (my_packet->visual == VI_NPC) {
				Monster* monster = GET_SINGLE(SceneManager)->GetInstance()->GetCurrentScene()->monsters[id] = new Monster();
				monster->SetPos(Vector{ (float)my_packet->x * 30, (float)my_packet->y * 30 });
			}
			else {
				Player* player = GET_SINGLE(SceneManager)->GetInstance()->GetCurrentScene()->players[id] = new Player();
				player->SetCellPos(VectorInt{ my_packet->x, my_packet->y }, true);
				player->SetState(PlayerState::Idle);
			}
		}

		break;
	}
	case static_cast<int>(SC_PACKET_ID::SC_MOVE_PLAYER):
	{
		SC_MOVE_PLAYER_PACKET* packet = reinterpret_cast<SC_MOVE_PLAYER_PACKET*>(p);
		int id = packet->id;
		if (id == myId) {
			avatar->SetPos(avatar->GetDestPos());
			avatar->SetCellPos(VectorInt{ packet->x, packet->y });
			avatar->SetDir(static_cast<Dir>(packet->direction));
			avatar->SetState(PlayerState::Move);
		}
		else {
			Player* player = GET_SINGLE(SceneManager)->GetInstance()->GetCurrentScene()->players[id];
			player->SetPos(player->GetDestPos());
			player->SetCellPos(VectorInt{ packet->x, packet->y });
			player->SetDir(static_cast<Dir>(packet->direction));
			player->SetState(PlayerState::Move);
		}		break;
	}

	case static_cast<int>(SC_PACKET_ID::SC_REMOVE_PLAYER):
	{
			break;
	}
	case static_cast<int>(SC_PACKET_ID::SC_CHAT):
	{
		SC_CHAT_PACKET* my_packet = reinterpret_cast<SC_CHAT_PACKET*>(p);
		HWND hListBox = GetDlgItem(_hwnd, 1000);
		wchar_t message[(CHAT_SIZE + NAME_SIZE) / 2];

		wchar_t name[NAME_SIZE / 2];
		MultiByteToWideChar(CP_ACP, 0, my_packet->name, -1, name, NAME_SIZE / 2);
		wchar_t text[CHAT_SIZE/2];
		MultiByteToWideChar(CP_ACP, 0, my_packet->mess, -1, text, CHAT_SIZE / 2);

		swprintf(message, NAME_SIZE + CHAT_SIZE, L"[%s] : %s", name, text);
	
		SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)message);
		SetDlgItemText(_hwnd, 2000, TEXT(""));
		SendMessage(hListBox, LB_SETCARETINDEX, (WPARAM)(SendMessage(hListBox, LB_GETCOUNT, 0, 0) - 1), 0);
		break;
	}
	default:
		printf("Unknown PACKET type [%d]\n", recvBuffer[1]);
		break;
	}
}

void NetworkManager::process_data(size_t io_byte)
{
	int remain_data = io_byte + prev_remain;
	char* p = recvBuffer;
	while (remain_data > 0) {
		int packet_size = p[0];
		if (packet_size <= remain_data) {
			ProcessPacket(p);
			p = p + packet_size;
			remain_data = remain_data - packet_size;
		}
		else break;
	}
	prev_remain = remain_data;
	if (remain_data > 0) {
		memcpy(recvBuffer, p, remain_data);
	}
}


void NetworkManager::SendChat(const char* message)
{
	CS_CHAT_PACKET packet;
	packet.size = strlen(message) + 2 + 1;
	packet.type = static_cast<char>(CS_PACKET_ID::CS_CHAT);
	memcpy_s(packet.mess, 100, message, 100);
	send(socket, reinterpret_cast<char*>(&packet), packet.size, 0);
}