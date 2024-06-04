#include <iostream>
#include "Server.h"
#include <vector>
#include <thread>
#include <mutex>
#include "Session.h"
#include "Monster.h"
#include <unordered_set>
#include <fstream>

void Server::Init()
{
	std::cout << "서버 실행중..." << std::endl;

	LoadCollision("collision.txt");
	initialize_npc();

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
	AcceptEx(_listen_socket, _client_socket, _accept_over._buff.GetBuff(), 0, addr_size + 16, addr_size + 16, 0, &_accept_over._over);
	
	_timerQueue.Init(_hiocp);
	std::cout << "서버 부팅 완료" << std::endl;
}

void Server::initialize_npc()
{
	std::cout << "NPC intialize begin.\n";
	for (int i = 0; i < NUM_NPC; ++i) {
		int npc_id = i + MAX_USER;
		objects[npc_id] = new Monster();

		int x = 0;
		int y = 0;
		while (true) {
			x = rand() % W_WIDTH;
			y = rand() % W_HEIGHT;
			if (can_go(x, y))
				break;
		}

		int sectorId = (x / SECTOR_SIZE) + ((y / SECTOR_SIZE) * MULTIPLY_ROW);
		objects[npc_id]->SetSectorId(sectorId);
		sectors[sectorId].AddPlayerList(npc_id);
		objects[npc_id]->SetState(ST_INGAME);

		char name[NAME_SIZE];
		sprintf_s(name, "M%d", i);
		objects[npc_id]->Init(x, y, npc_id, name);
		objects[npc_id]->SetIsNpc(true);
	}
	std::cout << "NPC initialize end.\n";
}

void Server::LoadCollision(const char* fileName)
{
	_collision = std::vector<std::vector<int>>(W_HEIGHT, std::vector<int>(W_WIDTH));

	std::wifstream ifs{ fileName };
	if (!ifs) {
		std::cout << "파일로드 실패" << std::endl;
		exit(-1);
	}

	for (int y = 0; y < W_HEIGHT; ++y)
	{
		std::wstring line;
		ifs >> line;

		for (int x = 0; x < W_WIDTH; ++x)
		{
			_collision[y][x] = line[x] - L'0';
		}
	}
	ifs.close();
}

bool Server::can_see(int objectID_1, int objectID_2)
{
	Pos pos1 = objects[objectID_1]->GetPosition();
	Pos pos2 = objects[objectID_2]->GetPosition();

	if (abs(pos1.x - pos2.x) > VIEW_RANGE) return false;
	return abs(pos1.y - pos2.y) <= VIEW_RANGE;
}

bool Server::can_go(int x, int y)
{
	return _collision[y][x] != 1;
}

void Server::process_packet(int id, char* packet)
{
	switch (packet[2]) {
	case CS_LOGIN: {
		CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet);
		Session* loginPlayer = reinterpret_cast<Session*>(objects[id]);
		loginPlayer->SetName(p->name);
		loginPlayer->SetPosition(54, 11/*rand() % W_WIDTH, rand() % W_HEIGHT*/);
		
		Pos playerPos = loginPlayer->GetPosition();
		int sectorId = (playerPos.x / SECTOR_SIZE) + ((playerPos.y / SECTOR_SIZE) * MULTIPLY_ROW);
		sectors[sectorId].AddPlayerList(id);

		loginPlayer->send_login_info_packet(VI_PLAYER);

		{
			std::lock_guard<std::mutex> ll{ loginPlayer->GetStateMutex() };
			loginPlayer->SetState(ST_INGAME);
		}

		std::unordered_set<int> playerList;
		int adj_index = 0;

		for (int i = 0; i < ADJ_COUNT; ++i) {
			adj_index = sectorId - adj_sector[i];
			if (adj_index < 0 || adj_index > SECTOR_COUNT - 1)
				continue;

			 sectors[adj_index].GetPlayerList(playerList);

			for (auto& pl_id : playerList) {
				Object& cl = *(objects[pl_id]);
				{
					std::lock_guard<std::mutex> ll(cl.GetStateMutex());
					if (ST_INGAME != cl.GetState()) continue;
				}
				if (pl_id == id) 
					continue;
				if (true == can_see(pl_id, id)) {

					// 이부분 함수로 뺴기 가능?
					if (true == cl.GetIsNpc()) {
						OVER_EXP* exover = new OVER_EXP;
						exover->_comp_type = OP_AI_HELLO;
						exover->_cause_player_id = id;
						PostQueuedCompletionStatus(_hiocp, 1, pl_id, &exover->_over);
						loginPlayer->send_add_player_packet(cl, VI_NPC);

						Monster* monster = reinterpret_cast<Monster*>(objects[pl_id]);
						if (false == monster->GetIsActive()) {
							if (true == monster->CASIsActive(false, true))
								_timerQueue.add_timer(pl_id, EV_RANDOM_MOVE, 1000);
							continue;
						}
						else {
							loginPlayer->send_add_player_packet(cl, VI_NPC);
						}
					}
					else {
						loginPlayer->send_add_player_packet(cl, VI_PLAYER);
						reinterpret_cast<Session*>(&cl)->send_add_player_packet(*loginPlayer, VI_PLAYER);
					}
				}
			}
		}
		break;
	}
	case CS_MOVE: {
		//std::cout << "CS_MOVE" << std::endl;

		CS_MOVE_PACKET* p = reinterpret_cast<CS_MOVE_PACKET*>(packet);
		Session* movePlayer = reinterpret_cast<Session*>(objects[id]);
		process_move(movePlayer, id, p->direction);
		break;
	}
	case CS_CHAT: {
		CS_CHAT_PACKET* p = reinterpret_cast<CS_CHAT_PACKET*>(packet);
		std::cout << p->mess;
		BroadCastChat(id, p->mess);
		break;
	}
	//case CS_PACKET_ID::CS_MOVE_STOP: {
	//	CS_MOVE_STOP_PACKET* p = reinterpret_cast<CS_MOVE_STOP_PACKET*>(packet);
	//	std::cout << "CS_MOVE_STOP" << std::endl;
	//	break;
	//}
	default:
		std::cout << "정체불명 패킷" << std::endl;
		break;
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
			std::cout << "아이디 할당 : " << client_id << std::endl;
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
			AcceptEx(_listen_socket, _client_socket, _accept_over._buff.GetBuff(), 0, addr_size + 16, addr_size + 16, 0, &_accept_over._over);
			break;
		}
		case OP_RECV: {
			Session* player = reinterpret_cast<Session*>(objects[key]);
			RecvBuffer* recvBuff = &(player->GetOverEXP()->_buff);
			int remain_data = num_bytes + recvBuff->GetPrevRemain();
			int frontIndex = recvBuff->GetRecvBuffFrontIndex();
			char* p = recvBuff->GetBuff(frontIndex);
			while (remain_data > 0) {
				unsigned short packet_size = (p[1] << 8) | p[0];
				if (packet_size <= remain_data) {
					process_packet(static_cast<int>(key), p);
					p = p + packet_size;
					remain_data = remain_data - packet_size;
				}
				else break;
			}
	
			recvBuff->SetPrevRemain(remain_data);
			recvBuff->SetAddRecvBuffRearIndex(num_bytes);
			recvBuff->SetAddRecvBuffForntIndex(num_bytes);
			if(remain_data > 0) { // 만족하면 rear와 front가 같지 않음
				if ((recvBuff->GetRecvBuffRearIndex()) == BUF_SIZE) {
					memcpy(recvBuff->GetBuff(), p, remain_data);
					recvBuff->SetRecvBuffForntIndex(0);
					recvBuff->SetRecvBuffRearIndex(remain_data);
				}
			}
			else { // rear와 front가 같다. 0으로 위치를 옮긴다. 
				recvBuff->SetRecvBuffForntIndex(0);
				recvBuff->SetRecvBuffRearIndex(0);
			}
			player->do_recv();
			break;
		}
		case OP_SEND:
			delete ex_over;
			break;
		case OP_NPC_MOVE:
			//std::cout << key << " OP_NPC_MOVE" << std::endl;
			Monster* monster = reinterpret_cast<Monster*>(objects[key]);

			std::unordered_set<int> prevPlayerList;
			GetNearPlayersList(key, prevPlayerList);

			if (prevPlayerList.size() == 0) {
				monster->SetIsActive(false); // 데이터 레이스 없음
				break;
			}
			_timerQueue.add_timer(key, EV_RANDOM_MOVE, 1000);
			Pos prevPos = monster->GetPosition();
			short x = 0;
			short y = 0;
			while (true) {
				// 사면이 막힌 위치에 몬스터가 생성되면 스레드 하나가 while문을 계속 돈다
				x = prevPos.x;
				y = prevPos.y;
				switch (rand() % 4) {
				case DIR_UP: if (y > 0) y--; break;
				case DIR_DOWN: if (y < W_HEIGHT - 1) y++; break;
				case DIR_LEFT: if (x > 0) x--; break;
				case DIR_RIGHT: if (x < W_WIDTH - 1) x++; break;
				}
				if (!can_go(x, y)) {
					continue;;
				}
				break;
			}
			monster->SetPosition(x, y);
			int sectorId = SetSectorId(*monster, key, x, y);

			std::unordered_set<int> newPlayerList;
			GetNearPlayersList(key, newPlayerList);

			for (auto& cl : newPlayerList) {
				Session* addPlayer = reinterpret_cast<Session*>(objects[cl]);
				if (0 == prevPlayerList.count(cl)) {
					addPlayer->send_add_player_packet(*(objects[key]), VI_NPC);				
				}
				else {
					addPlayer->send_move_packet(*(objects[key]), 0);
				}
			}
			for (auto& cl : prevPlayerList) {
				if (0 == newPlayerList.count(cl)) {
					Session* removePlayer = reinterpret_cast<Session*>(objects[cl]);
					removePlayer->send_remove_player_packet(key);
				}
			}

			break;
		}
	
	}
}

void Server::disconnect(int key)
{
}

void Server::BroadCastChat(int id, char* p)
{
	SC_CHAT_PACKET chatPacket;
	chatPacket.size = sizeof(SC_CHAT_PACKET) - CHAT_SIZE + strlen(p) + 1;
	chatPacket.type = static_cast<int>(SC_CHAT);
	chatPacket.id = id;
	memcpy_s(chatPacket.mess, CHAT_SIZE, p, CHAT_SIZE);

	for (int i = 0; i < MAX_USER; ++i) {
		Session* player = reinterpret_cast<Session*>(objects[i]);
		if (player->GetState() != ST_INGAME) continue;
			player->do_send(&chatPacket);
	}
}

void Server::process_move(Session* movePlayer, int id, char direction)
{
	Pos prevPos = movePlayer->GetPosition();
	short x = prevPos.x;
	short y = prevPos.y;
	switch (direction) {
	case DIR_UP: if (y > 0) y--; break;
	case DIR_DOWN: if (y < W_HEIGHT - 1) y++; break;
	case DIR_LEFT: if (x > 0) x--; break;
	case DIR_RIGHT: if (x < W_WIDTH - 1) x++; break;
	}
	if (!can_go(x, y)) {
		return;
	}
	movePlayer->SetPosition(x, y);

	int sectorId = SetSectorId(*movePlayer, id, x, y);

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

		sectors[adj_index].GetPlayerList(playerList);

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

					Monster* monster = reinterpret_cast<Monster*>(objects[pl_id]);
					if (false == monster->GetIsActive()) {
						bool input = false;
						if (true == monster->CASIsActive(false, true))
							_timerQueue.add_timer(pl_id, EV_RANDOM_MOVE, 1000);
					}
				}
			}
		}
	}

	movePlayer->send_move_packet(*(objects[id]), direction);
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
			if (false == addPlayer->GetIsNpc()) {
				addPlayer->send_move_packet(*(objects[id]), direction);
			}
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

void Server::GetNearPlayersList(int id, std::unordered_set<int>& list)
{ 
	std::unordered_set<int> playerList;
	int adj_index = objects[id]->GetSectorId();
	for (int i = 0; i < ADJ_COUNT; ++i)
	{
		int index = adj_index - adj_sector[i];
		if (index < 0 || index > SECTOR_COUNT - 1)
			continue;

		sectors[index].GetPlayerList(playerList);

		for (auto& p : playerList) {
			Object* s = reinterpret_cast<Session*>(objects[p]);
			if (s->GetIsNpc()) continue;
			if (s->GetState() != ST_INGAME) continue;
			if (can_see(s->GetId(), id))
				list.insert(s->GetId());
		}
	}
}

int Server::SetSectorId(Object& obj, int id, int x, int y)
{
	int sectorId = (x / SECTOR_SIZE) + ((y / SECTOR_SIZE) * MULTIPLY_ROW);

	int currentSectorId = obj.GetSectorId();
	if (sectorId != currentSectorId) {
		sectors[currentSectorId].RemovePlayerList(id);

		obj.SetSectorId(sectorId);
		sectors[sectorId].AddPlayerList(id);
	}

	return sectorId;
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
