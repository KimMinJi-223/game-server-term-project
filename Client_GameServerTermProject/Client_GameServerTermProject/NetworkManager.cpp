#include <WS2tcpip.h>
#include <MSWSock.h>
#include "Player.h"
#include "NetworkManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Actor.h"

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

void NetworkManager::Init()
{
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

	if(recvLen > 0)
		process_data(recvLen);
}

void NetworkManager::ProcessPacket(char* p)
{
	static bool first_time = true;
	switch (p[1])
	{
	case static_cast<int>(SC_PACKET_ID::SC_LOGIN_INFO):
	{
		SC_LOGIN_INFO_PACKET* packet = reinterpret_cast<SC_LOGIN_INFO_PACKET*>(p);
		myId = packet->id;
		//Pos newPos = { (float)packet->x, (float)packet->y };
		VectorInt newPos2 = { packet->x, packet->y };

		avatar->SetCellPos(newPos2, true);

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
			Player* player = GET_SINGLE(SceneManager)->GetInstance()->GetCurrentScene()->players[id] = new Player();
			player->SetCellPos(VectorInt{ my_packet->x, my_packet->y }, true);
			player->SetState(PlayerState::Idle);
		}

		break;
	}
	case static_cast<int>(SC_PACKET_ID::SC_MOVE_PLAYER):
	{
		SC_MOVE_PLAYER_PACKET* my_packet = reinterpret_cast<SC_MOVE_PLAYER_PACKET*>(p);
		int id = my_packet->id;
		if (id == myId) {
			avatar->SetPos(avatar->GetDestPos());
			avatar->SetCellPos(VectorInt{ my_packet->x, my_packet->y });
			avatar->SetDir(static_cast<Dir>(my_packet->direction));
			avatar->SetState(PlayerState::Move);
		}
		else {
			Player* player = GET_SINGLE(SceneManager)->GetInstance()->GetCurrentScene()->players[id];
			player->SetPos(player->GetDestPos());
			player->SetCellPos(VectorInt{ my_packet->x, my_packet->y });
			player->SetDir(static_cast<Dir>(my_packet->direction));
			player->SetState(PlayerState::Move);
		}		break;
	}

	/*case static_cast<int>(SC_PACKET_ID::SC_REMOVE_PLAYER):
	{
		SC_REMOVE_PLAYER_PACKET* my_packet = reinterpret_cast<SC_REMOVE_PLAYER_PACKET*>(p);
		int other_id = my_packet->id;
		if (other_id == g_myid) {
			avatar.hide();
		}
		else {
			players.erase(other_id);
		}
		break;
	}*/
	/*case static_cast<int>(SC_PACKET_ID::SC_CHAT):
	{
		SC_CHAT_PACKET* my_packet = reinterpret_cast<SC_CHAT_PACKET*>(p);
		int other_id = my_packet->id;
		if (other_id == g_myid) {
			avatar.set_chat(my_packet->mess);
		}
		else {
			players[other_id].set_chat(my_packet->mess);
		}

		break;
	}*/
	default:
		printf("Unknown PACKET type [%d]\n", recvBuffer[1]);
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
