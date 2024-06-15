#include "pch.h"
#include "Player.h"
#include "Monster.h"
#include "NetworkManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Actor.h"

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
	p.type = static_cast<char>(CS_LOGIN);
	avatar->SetName("hello");
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
	switch (p[2])
	{
	case SC_LOGIN_INFO:
	{
		SC_LOGIN_INFO_PACKET* packet = reinterpret_cast<SC_LOGIN_INFO_PACKET*>(p);
		myId = packet->id;
		avatar->_id = myId;
		VectorInt pos = { packet->x, packet->y };
		avatar->SetHp(packet->hp);
		avatar->SetMaxHp(packet->max_hp);
		avatar->SetEXP(packet->exp);
		avatar->SetLevel(packet->level);
		avatar->SetCellPos(pos, true);
	}
	break;
	case SC_ADD_OBJECT:
	{
		SC_ADD_OBJECT_PACKET* packet = reinterpret_cast<SC_ADD_OBJECT_PACKET*>(p);
		int id = packet->id;

		if (id == myId) {
			avatar->SetCellPos(VectorInt{ packet->x, packet->y }, true);
			avatar->SetState(State::Idle);
		}
		else {
			if (id < MAX_USER) {
				Player* player = GET_SINGLE(SceneManager)->GetInstance()->GetCurrentScene()->players[id] = new Player();
				player->SetCellPos(VectorInt{ packet->x, packet->y }, true);
				player->SetState(State::Idle);
				player->SetName(packet->name);
				player->SetHp(packet->hp);
				player->SetLevel(packet->level);
			}
			else { 
				Monster* monster = GET_SINGLE(SceneManager)->GetInstance()->GetCurrentScene()->monsters[id] = new Monster(packet->visual);
				monster->SetCellPos(VectorInt{ packet->x, packet->y }, true);
				monster->SetState(State::Idle);
				monster->SetName(packet->name);
				monster->SetHp(packet->hp);
				monster->SetLevel(packet->level);
			}
		}

		break;
	}
	case SC_MOVE_OBJECT:
	{
		SC_MOVE_OBJECT_PACKET* packet = reinterpret_cast<SC_MOVE_OBJECT_PACKET*>(p);
		int id = packet->id;
		if (id == myId) {
			avatar->SetPos(avatar->GetDestPos());
			avatar->SetCellPos(VectorInt{ packet->x, packet->y });
			avatar->SetDir(static_cast<Dir>(packet->direction));
			avatar->SetState(State::Move);
		}
		else {
			if (id < MAX_USER) {
				auto player = GET_SINGLE(SceneManager)->GetInstance()->GetCurrentScene()->players.find(id);
				if (player == GET_SINGLE(SceneManager)->GetInstance()->GetCurrentScene()->players.end())
					break;
				player->second->SetPos(player->second->GetDestPos());
				player->second->SetCellPos(VectorInt{ packet->x, packet->y });
				player->second->SetDir(static_cast<Dir>(packet->direction));
				player->second->SetState(State::Move);
			}
			else {
				auto monster = GET_SINGLE(SceneManager)->GetInstance()->GetCurrentScene()->monsters.find(id);
				if (monster == GET_SINGLE(SceneManager)->GetInstance()->GetCurrentScene()->monsters.end())
					break;
				monster->second->SetPos(monster->second->GetDestPos());
				monster->second->SetCellPos(VectorInt{ packet->x, packet->y });
				monster->second->SetDir(static_cast<Dir>(packet->direction));
				monster->second->SetState(State::Move);
			}
		}		break;
	}

	case SC_REMOVE_OBJECT:
	{
		SC_REMOVE_OBJECT_PACKET* packet = reinterpret_cast<SC_REMOVE_OBJECT_PACKET*>(p);
		int id = packet->id;
		if (id == myId) {
			break;
		}
		if (id < MAX_USER) {
			GET_SINGLE(SceneManager)->GetInstance()->GetCurrentScene()->players.erase(id);
		}
		else {
			unordered_map <int, Monster*>& mon = GET_SINGLE(SceneManager)->GetInstance()->GetCurrentScene()->monsters;
			GET_SINGLE(SceneManager)->GetInstance()->GetCurrentScene()->monsters.erase(id);
		}
			break;
	}
	case SC_CHAT:
	{
		SC_CHAT_PACKET* packet = reinterpret_cast<SC_CHAT_PACKET*>(p);
		HWND hListBox = GetDlgItem(_hwnd, 1000);
		wchar_t message[(CHAT_SIZE + NAME_SIZE) / 2];

		wchar_t name[NAME_SIZE / 2];
		MultiByteToWideChar(CP_ACP, 0, avatar->GetName(), -1, name, NAME_SIZE / 2);
		wchar_t text[CHAT_SIZE/2];
		MultiByteToWideChar(CP_ACP, 0, packet->mess, -1, text, CHAT_SIZE / 2);
		swprintf(message, NAME_SIZE + CHAT_SIZE, L"[%s] : %s", name, text);
	
		SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)message);
		SetDlgItemText(_hwnd, 2000, TEXT(""));
		SendMessage(hListBox, LB_SETCARETINDEX, (WPARAM)(SendMessage(hListBox, LB_GETCOUNT, 0, 0) - 1), 0);
		break;
	}
	case SC_EXP_CHANGE:
	{
		SC_EXP_CHANGE_PACKET* packet = reinterpret_cast<SC_EXP_CHANGE_PACKET*>(p);
		avatar->SetEXP(packet->exp);
		break;
	}
	case SC_HP_CHANGE:
	{
		SC_HP_CHANGE_PACKET* packet = reinterpret_cast<SC_HP_CHANGE_PACKET*>(p);
		int id = packet->id;

		if (id < MAX_USER) {
			auto player = GET_SINGLE(SceneManager)->GetInstance()->GetCurrentScene()->players.find(id);
			if (player == GET_SINGLE(SceneManager)->GetInstance()->GetCurrentScene()->players.end())
				break;
			player->second->SetHp(packet->hp);
		}
		else {
			auto monster = GET_SINGLE(SceneManager)->GetInstance()->GetCurrentScene()->monsters.find(id);
			if (monster == GET_SINGLE(SceneManager)->GetInstance()->GetCurrentScene()->monsters.end())
				break;
			monster->second->SetHp(packet->hp);
		}
		break;
	}
	case SC_LEVEL_CHANGE:
	{
		SC_LEVEL_CHANGE_PACKET* packet = reinterpret_cast<SC_LEVEL_CHANGE_PACKET*>(p);
		int id = packet->id;
		if (id == myId) {
			avatar->SetLevel(packet->level);
			avatar->SetEXP(packet->exp);
			break;
		}
		auto player = GET_SINGLE(SceneManager)->GetInstance()->GetCurrentScene()->players.find(id);
		if (player == GET_SINGLE(SceneManager)->GetInstance()->GetCurrentScene()->players.end())
			break;
		player->second->SetLevel(packet->level);
		player->second->SetEXP(packet->exp);

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
		int packet_size = *reinterpret_cast<unsigned short*>(p);
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
	packet.size = strlen(message) + 3 + 1;
	packet.type = static_cast<char>(CS_CHAT);
	memcpy_s(packet.mess, CHAT_SIZE, message, CHAT_SIZE);
	send(socket, reinterpret_cast<char*>(&packet), packet.size, 0);
}