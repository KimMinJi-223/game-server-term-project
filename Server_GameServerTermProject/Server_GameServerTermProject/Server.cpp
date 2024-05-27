#include <iostream>
#include "Server.h"
#include <vector>
#include <thread>
#include <mutex>
#include "Session.h"
#include "Monster.h"
#include <unordered_set>

void Server::Init()
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);

	for (int i = 0; i < MAX_USER; ++i)
		objects[i] = new Session();

	_listen_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT_NUM);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(_listen_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(_listen_socket, SOMAXCONN);
	SOCKADDR_IN cl_addr;
	int addr_size = sizeof(cl_addr);
	_hiocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(_listen_socket), _hiocp, 9999, 0);
	_client_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	_accept_over._comp_type = OP_ACCEPT;
	AcceptEx(_listen_socket, _client_socket, _accept_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &_accept_over._over);
}

bool Server::can_see(int objectID_1, int objectID_2)
{
	Pos pos1 = objects[objectID_1]->GetPosition();
	Pos pos2 = objects[objectID_2]->GetPosition();

	if (abs(pos1.x - pos2.x) > VIEW_RANGE) return false;
	return abs(pos1.y - pos2.y) <= VIEW_RANGE;
}

void Server::process_packet(int id, char* packet)
{
	switch (static_cast<CS_PACKET_ID>(packet[1])) {
	case CS_PACKET_ID::CS_LOGIN: {
		CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet);
		Session* loginPlayer = reinterpret_cast<Session*>(objects[id]);
		loginPlayer->SetName(p->name);
		loginPlayer->SetPosition(rand() % W_WIDTH, rand() % W_HEIGHT);
		
		Pos playerPos = loginPlayer->GetPosition();
		int sectorId = (playerPos.x / SECTOR_SIZE) + ((playerPos.y / SECTOR_SIZE) * MULTIPLY_ROW);
		//sectors[sectorId].WriteLock();
		sectors[sectorId].AddPlayerList(id);
		//sectors[sectorId].WriteUnLock();

		loginPlayer->send_login_info_packet(VI_PLAYER);
		{
			std::lock_guard<std::mutex> ll{ loginPlayer->GetStateMutex() };
			loginPlayer->SetState(ST_INGAME);
		}

		std::unordered_set<int> playerList;
		int adj_index = 0;

		for (int i = 0; i < ADJ_COUNT; ++i)
		{
			adj_index = sectorId - adj_sector[i];
			if (adj_index < 0 || adj_index > SECTOR_COUNT - 1)
				continue;

			sectors[adj_index].ReadLock();
			playerList = sectors[adj_index]._player_list;
			sectors[adj_index].ReadUnLock();

			for (auto& pl_id : playerList) {
				Object& cl = *(objects[pl_id]);
				{
					std::lock_guard<std::mutex> ll(cl.GetStateMutex());
					if (ST_INGAME != cl.GetState()) continue;
				}
				if (pl_id == id) continue;
				if (true == can_see(pl_id, id)) {
					if (true == cl.GetIsNpc())
					{
						OVER_EXP* exover = new OVER_EXP;
						exover->_comp_type = OP_AI_HELLO;
						exover->_cause_player_id = id;
						PostQueuedCompletionStatus(_hiocp, 1, pl_id, &exover->_over);
						loginPlayer->send_add_player_packet(cl, VI_NPC);

						Monster* monster = reinterpret_cast<Monster*>(objects[id]);
						if (false == monster->GetIsActive()) {
							bool input = false;
							if (true == monster->CASIsActive(false, true))
								_timerQueue.add_timer(pl_id, EV_RANDOM_MOVE, 1000);
							continue;
						}
						else if (true == monster->GetIsActive())
							loginPlayer->send_add_player_packet(cl, VI_NPC);
					}
					else {
						loginPlayer->send_add_player_packet(cl, VI_PLAYER);
						reinterpret_cast<Session*>(&cl)->send_add_player_packet(*(objects[id]), VI_PLAYER);
					}
				}
			}
		}
		break;
	}
	case CS_PACKET_ID::CS_MOVE: {
		CS_MOVE_PACKET* p = reinterpret_cast<CS_MOVE_PACKET*>(packet);
		Session* movePlayer = reinterpret_cast<Session*>(objects[id]);
		movePlayer->_last_move_time = p->move_time;
		Pos prevPos = movePlayer->GetPosition();
		short x = prevPos.x;
		short y = prevPos.y;
		switch (p->direction) {
		case 0: if (y > 0) y--; break;
		case 1: if (y < W_HEIGHT - 1) y++; break;
		case 2: if (x > 0) x--; break;
		case 3: if (x < W_WIDTH - 1) x++; break;
		}
		movePlayer->SetPosition(x, y);


		int sectorId = (x / SECTOR_SIZE) + ((y / SECTOR_SIZE) * MULTIPLY_ROW);

		int currentSectorId = movePlayer->GetSectorId();
		if (sectorId != currentSectorId) {
			//sectors[currentSectorId].WriteLock();
			sectors[currentSectorId].RemovePlayerList(id);
			//sectors[currentSectorId].WriteUnLock();

			movePlayer->SetSectorId(sectorId);
			//sectors[sectorId].WriteLock();
			sectors[sectorId].AddPlayerList(id);
			//sectors[sectorId].WriteUnLock();
		}

	
		std::unordered_set<int> old_vl;
		movePlayer->GetRefViewList(old_vl);
		std::unordered_set<int> new_vl;

		std::unordered_set<int> playerList;
		int adj_index = 0;
		for (int i = 0; i < ADJ_COUNT; ++i)
		{
			adj_index = sectorId - adj_sector[i];
			if (adj_index < 0 || adj_index > SECTOR_COUNT - 1)
				continue;

			sectors[adj_index].ReadLock();
			playerList = sectors[adj_index]._player_list;
			sectors[adj_index].ReadUnLock();

			for (auto& pl_id : playerList) {
				Object& cl = *(objects[pl_id]);

				if (cl.GetState() != ST_INGAME) continue;
				if (pl_id == id) continue;
				if (true == can_see(pl_id, id)) {
					new_vl.insert(pl_id);
					if ((true == cl.GetIsNpc())) {
						OVER_EXP* exover = new OVER_EXP;
						exover->_comp_type = OP_AI_HELLO;
						exover->_cause_player_id = id;
						PostQueuedCompletionStatus(_hiocp, 1, pl_id, &exover->_over);

						Monster* monster = reinterpret_cast<Monster*>(objects[id]);
						if (false == monster->GetIsActive()) {
							bool input = false;
							if (true == monster->CASIsActive(false, true))
								_timerQueue.add_timer(pl_id, EV_RANDOM_MOVE, 1000);
						}
					}
				}
			}
		}

		movePlayer->send_move_packet(*(objects[id]));
		// ADD_PLAYER
		for (auto& cl : new_vl) {
			Session* addPlayer = reinterpret_cast<Session*>(objects[cl]);
			char c_visual = VI_PLAYER;
			if (0 == old_vl.count(cl)) {
				if (false == addPlayer->GetIsNpc()) {
					addPlayer->send_add_player_packet(*(objects[id]), VI_PLAYER);
				}
				else
					c_visual = VI_NPC;
				movePlayer->send_add_player_packet(*(objects[cl]), c_visual);
			}
			else {
				// MOVE_PLAYER
				if (false == addPlayer->GetIsNpc())
					addPlayer->send_move_packet(*(objects[id]));
			}
		}
		// REMOVE_PLAYER
		for (auto& cl : old_vl) {
			if (0 == new_vl.count(cl)) {
				if (false == objects[cl]->GetIsNpc()) {
					Session* removePlayer = reinterpret_cast<Session*>(objects[cl]);
					removePlayer->send_remove_player_packet(id);
				}
				movePlayer->send_remove_player_packet(cl);
			}
		}

	}
	}
}

void Server::WorkerThread()
{
	while (true) {
		DWORD num_bytes;
		ULONG_PTR key;
		WSAOVERLAPPED* over = nullptr;
		BOOL ret = GetQueuedCompletionStatus(_hiocp, &num_bytes, &key, &over, INFINITE);
		OVER_EXP* ex_over = reinterpret_cast<OVER_EXP*>(over);
		if (FALSE == ret) {
			if (ex_over->_comp_type == OP_ACCEPT) 
				std::cout << "Accept Error";
			else {
				std::cout << "GQCS Error on client[" << key << "]\n";
				disconnect(static_cast<int>(key));
				if (ex_over->_comp_type == OP_SEND) delete ex_over;
				continue;
			}
		}

		if ((0 == num_bytes) && ((ex_over->_comp_type == OP_RECV) || (ex_over->_comp_type == OP_SEND))) {
			disconnect(static_cast<int>(key));
			if (ex_over->_comp_type == OP_SEND) delete ex_over;
			continue;
		}

		switch (ex_over->_comp_type) {
		case OP_ACCEPT: {
			int client_id = get_new_client_id();
			if (client_id != -1) {
				{
					std::lock_guard<std::mutex> ll(objects[client_id]->GetStateMutex());
					objects[client_id]->SetState(ST_ALLOC);
				}
				Session* newPlayer = reinterpret_cast<Session*>(objects[client_id]);
				newPlayer->Init(0, 0, client_id, "", _client_socket);
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(_client_socket), _hiocp, client_id, 0);
				newPlayer->do_recv();
				_client_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			}
			else {
				std::cout << "Max user exceeded.\n";
			}
			ZeroMemory(&_accept_over._over, sizeof(_accept_over._over));
			int addr_size = sizeof(SOCKADDR_IN);
			AcceptEx(_listen_socket, _client_socket, _accept_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &_accept_over._over);
			break;
		}
		case OP_RECV: {
			Session* player = reinterpret_cast<Session*>(objects[key]);
			int remain_data = num_bytes + player->GetPrevRemain();
			char* p = ex_over->_send_buf;
			while (remain_data > 0) {
				int packet_size = p[0];
				if (packet_size <= remain_data) {
					process_packet(static_cast<int>(key), p);
					p = p + packet_size;
					remain_data = remain_data - packet_size;
				}
				else break;
			}
			player->SetPrevRemain(remain_data);
			if (remain_data > 0) {
				memcpy(ex_over->_send_buf, p, remain_data);
			}
			player->do_recv();
			break;
		}
		case OP_SEND:
			delete ex_over;
			break;
		}
	}
}

void Server::disconnect(int key)
{
}

int Server::get_new_client_id()
{
	for (int i = 0; i < MAX_USER; ++i) {
		std::lock_guard<std:: mutex> ll{ objects[i]->GetStateMutex() };
		if (objects[i]->GetState() == ST_FREE)
			return i;
	}
	return -1;
}
