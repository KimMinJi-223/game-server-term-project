#include <iostream>
#include "Server.h"
#include <vector>
#include <thread>
#include <mutex>
#include "Session.h"
#include "Monster.h"
#include <unordered_set>
#include <fstream>
#include <map>
#include <queue>
#include <codecvt>
std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

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
	_dbQueue.Init(_hiocp);

	std::cout << "서버 부팅 완료" << std::endl;
}

void Server::initialize_npc()
{
	std::cout << "NPC intialize begin.\n";
	for (int i = 0; i < NUM_NPC; ++i) {
		int npc_id = i + MAX_USER;
		Monster* monster = new Monster();
		objects[npc_id] = monster;

		int x = 0;
		int y = 0;

		while (true) {
			x = rand() % W_WIDTH;
			y = rand() % W_HEIGHT;
			if (CanGo(x, y))
				break;
		}

		monster->Init(npc_id, x, y);

		int sectorId = (x / SECTOR_SIZE) + ((y / SECTOR_SIZE) * MULTIPLY_ROW);
		objects[npc_id]->SetSectorId(sectorId);
		sectors[sectorId].AddPlayerList(npc_id);
		objects[npc_id]->SetState(ST_INGAME);
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

	for (int y = 0; y < 40; ++y)
	{
		std::wstring line;
		ifs >> line;

		for (int x = 0; x < 40; ++x)
		{
			for (int i = 0; i < 50; ++i) {
				for (int j = 0; j < 50; ++j) {
					_collision[y + i * 40][x + j * 40] = line[x] - L'0';
				}
			}
		}
	}

	ifs.close();
}

bool Server::CanSee(int objectID_1, int objectID_2)
{
	Pos pos1 = objects[objectID_1]->GetPosition();
	Pos pos2 = objects[objectID_2]->GetPosition();

	if (abs(pos1.x - pos2.x) > VIEW_RANGE) return false;
	return abs(pos1.y - pos2.y) <= VIEW_RANGE;
}

bool Server::CanGo(int x, int y)
{
	return _collision[y][x] != 1;
}

int Server::API_GetPosX(lua_State* L)
{
	int id = (int)lua_tointeger(L, -1);
	lua_pop(L, 2);
	int x = Server::GetInstance()->objects[id]->GetPosition().x;
	lua_pushnumber(L, x);
	return 1;
}

int Server::API_GetPosY(lua_State* L)
{
	int id = (int)lua_tointeger(L, -1);
	lua_pop(L, 2);
	int y = Server::GetInstance()->objects[id]->GetPosition().y;
	lua_pushnumber(L, y);
	return 1;
}

int Server::API_AStarStart(lua_State* L)
{
	int monsterId = (int)lua_tointeger(L, -2);
	int targetId = (int)lua_tointeger(L, -1);
	lua_pop(L, 3);

	Monster* monster = reinterpret_cast<Monster*>(Server::GetInstance()->objects[monsterId]);
	monster->SetISAIMove(true);
	monster->SetTarget(targetId);
	return 0;
}

int Server::API_AStarEnd(lua_State* L)
{
	int monsterId = (int)lua_tointeger(L, -1);
	lua_pop(L, 2);
	Monster* monster = reinterpret_cast<Monster*>(Server::GetInstance()->objects[monsterId]);

	if (monster->GetIsRoaming())
		Server::GetInstance()->GetTImer()->AddTaskTimer(monsterId, monsterId, EV_RANDOM_MOVE, 1000);

	monster->SetISAIMove(false);
	monster->SetTarget(-1);
	Pos pos = monster->GetPosition();
	if (pos.x >= 0 && pos.y >= 0)
		monster->SetSpawnPos(pos);

	return 0;
}

int Server::API_AddTimer(lua_State* L)
{
	int monsterId = (int)lua_tointeger(L, -4);
	int targetId = (int)lua_tointeger(L, -3);
	int timerType = (int)lua_tointeger(L, -2);
	int time = (int)lua_tointeger(L, -1);
	lua_pop(L, 5);

	Server::GetInstance()->GetTImer()->AddTaskTimer(monsterId, targetId, static_cast<EVENT_TYPE>(timerType), time);
	if (static_cast<EVENT_TYPE>(timerType) == EV_AI_MOVE) {
		Server::GetInstance()->GetTImer()->AddTaskTimer(monsterId, targetId, EV_AI_LUA, time);
	}
	return 0;
}

void Server::process_packet(int id, char* packet)
{
	switch (packet[2]) {
	case CS_LOGIN: {
		processLoginRequest(id, packet);
		break;
	}
	case CS_MOVE: {
		processMoveRequest(id, packet);
		break;
	}
	case CS_CHAT: {
		CS_CHAT_PACKET* p = reinterpret_cast<CS_CHAT_PACKET*>(packet);
		BroadCastChat(id, p->mess);
		break;
	}
	case CS_ATTACK: {
		processAttackRequest(id, packet);

		break;
	}
	case CS_A_SKILL: {
		processSkillRequest(id, packet);
		break;
	}
	default:
		std::cout << "정체불명 패킷" << std::endl;
		break;
	}

}

void Server::WorkerThread()
{
	while (true) {
		DWORD numBytes;
		ULONG_PTR key;
		WSAOVERLAPPED* over = nullptr;
		BOOL ret = GetQueuedCompletionStatus(_hiocp, &numBytes, &key, &over, INFINITE);
		OVER_EXP* ex_over = reinterpret_cast<OVER_EXP*>(over);
		if (FALSE == ret) {
			if (ex_over->_comp_type == OP_ACCEPT)
				std::cout << "Accept Error";
			else {
				//std::cout << "GQCS Error on client[" << key << "]\n";
				disconnect(static_cast<int>(key));
				if (ex_over->_comp_type == OP_SEND) delete ex_over;
				continue;
			}
		}

		if ((0 == numBytes) && ((ex_over->_comp_type == OP_RECV) || (ex_over->_comp_type == OP_SEND))) {
			disconnect(static_cast<int>(key));
			if (ex_over->_comp_type == OP_SEND) delete ex_over;
			continue;
		}

		switch (ex_over->_comp_type) {
		case OP_ACCEPT: {
			int client_id = GetNewClientId();
			if (client_id != -1) {
				{
					std::lock_guard<std::mutex> ll(objects[client_id]->GetStateMutex());
					objects[client_id]->SetState(ST_ALLOC);
				}
				Session* newPlayer = reinterpret_cast<Session*>(objects[client_id]);
				newPlayer->Init(0, 0, client_id, "", _client_socket);
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(_client_socket), _hiocp, client_id, 0);
				newPlayer->DoRecv();
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
		case OP_LOGIN:
		{
			Session* player = reinterpret_cast<Session*>(objects[key]);
			Pos pos = player->GetPosition();

			while (true) {
				if (CanGo(pos.x, pos.y)) {
					break;
				}
				pos.x = rand() % W_WIDTH;
				pos.y = rand() % W_HEIGHT;

			}

			player->SetPosition(pos.x, pos.y);

			//std::cout << pos.x << " " << pos.y << std::endl;

			player->SendLoginInfoPacket(VI_PLAYER);

			int sectorId = (pos.x / SECTOR_SIZE) + ((pos.y / SECTOR_SIZE) * MULTIPLY_ROW);
			sectors[sectorId].AddPlayerList(key);

			{
				std::lock_guard<std::mutex> ll{ player->GetStateMutex() };
				player->SetState(ST_INGAME);
			}

			SpawnPlayer(key, sectorId);

			if (player->GetHp() < player->GetMaxHp()) {
				if (player->CASIsHeal(false, true))
					_timerQueue.AddTaskTimer(key, -1, EV_HEAL, 5000);
			}
		}
		delete ex_over;
		break;
		case OP_RECV: {
			Session* player = reinterpret_cast<Session*>(objects[key]);
			RecvBuffer* recvBuff = &(player->GetOverEXP()->_buff);
			int remainData = numBytes + recvBuff->GetPrevRemain();
			int frontIndex = recvBuff->GetFrontIndex();
			char* p = recvBuff->GetBuff(frontIndex);
			int processDate = 0;

			while (remainData > 0) {
				unsigned short packetSize = *reinterpret_cast<unsigned short*>(p);
				if (packetSize <= remainData) {
					process_packet(static_cast<int>(key), p);
					p = p + packetSize;
					remainData = remainData - packetSize;
					processDate += packetSize;
				}
				else break;
			}

			recvBuff->SetPrevRemain(remainData);
			
			if (remainData > 0) {
				if ((recvBuff->GetRecvBuffRearIndex()) >= BUF_INDEX_LIMIT) {
					memcpy(recvBuff->GetBuff(), p, remainData);
					recvBuff->SetFrontIndex(0);
					recvBuff->SetRecvBuffRearIndex(remainData);
				}
				else {
					recvBuff->SetAddFrontIndex(processDate);
					recvBuff->SetAddRecvBuffRearIndex(numBytes);
				}
			}
			else { 
				recvBuff->SetFrontIndex(0);
				recvBuff->SetRecvBuffRearIndex(0);
			}
			player->DoRecv();
		}
			break;

		case OP_SEND:
			delete ex_over;
			break;
		case OP_NPC_MOVE: {
			Monster* monster = reinterpret_cast<Monster*>(objects[key]);
			delete ex_over;
			// A* 중이면 랜덤이동 못하게
			if (monster->GetISAIMove()) {
				break;
			}
			std::unordered_set<int> prevPlayerList;
			GetNearPlayersList(key, prevPlayerList);

			// 주변에 아무도 없으면 비활성하기
			if (prevPlayerList.size() == 0) {
				monster->SetIsActive(false);
				break;
			}

			int x = 0;
			int y = 0;

			monster->move(x, y);

			int sectorId = SetSectorId(*monster, key, x, y);

			std::unordered_set<int> newPlayerList;
			GetNearPlayersList(key, newPlayerList);

			for (auto& cl : newPlayerList) {
				Session* addPlayer = reinterpret_cast<Session*>(objects[cl]);
				if (0 == prevPlayerList.count(cl)) {
					addPlayer->SendAddPlayerPacket(*(objects[key]), monster->GetMonsterType());
				}
				else {
					addPlayer->SendMovePacket(*(objects[key]), objects[key]->GetDir());
				}
			}
			for (auto& cl : prevPlayerList) {
				if (0 == newPlayerList.count(cl)) {
					Session* removePlayer = reinterpret_cast<Session*>(objects[cl]);
					removePlayer->SendRemovePlayerPacket(key);
				}
			}
			break;
		}
		case OP_ASTAR_MOVE: {
			delete ex_over;
			Monster* monster = reinterpret_cast<Monster*>(objects[key]);
			if (!monster->GetISAIMove()) {
				continue;
			}

			std::unordered_set<int> prevPlayerList;
			GetNearPlayersList(key, prevPlayerList);

			// 주변에 아무도 없으면 비활성하기
			if (prevPlayerList.size() == 0) {
				monster->SetIsActive(false);
				break;
			}

			int x = 0;
			int y = 0;
			AStar(x, y, key);

			int sectorId = SetSectorId(*monster, key, x, y);

			std::unordered_set<int> newPlayerList;
			GetNearPlayersList(key, newPlayerList);

			for (auto& cl : newPlayerList) {
				Session* addPlayer = reinterpret_cast<Session*>(objects[cl]);
				if (0 == prevPlayerList.count(cl)) {
					addPlayer->SendAddPlayerPacket(*(objects[key]), monster->GetMonsterType());
				}
				else {
					addPlayer->SendMovePacket(*(objects[key]), objects[key]->GetDir());
				}
			}
			for (auto& cl : prevPlayerList) {
				if (0 == newPlayerList.count(cl)) {
					Session* removePlayer = reinterpret_cast<Session*>(objects[cl]);
					removePlayer->SendRemovePlayerPacket(key);
				}
			}
			break;
		}
		case OP_CAN_ASTAR: {
			Monster* monster = reinterpret_cast<Monster*>(objects[key]);
			Pos causePos = objects[ex_over->_cause_player_id]->GetPosition();
			monster->isDoAStar(ex_over->_cause_player_id, causePos.x, causePos.y);
			delete ex_over;
			break;
		}
		case OP_NPC_ATTACK: {
			Monster* monster = reinterpret_cast<Monster*>(objects[key]);
			int AttackedId = ex_over->_cause_player_id;
			Session* player = reinterpret_cast<Session*>(objects[AttackedId]);
			int damageValue = monster->GetPower();
			bool isSuccess;
			int remainingHp = player->Damage(damageValue, isSuccess);
			if (isSuccess) {
				// 공격성공
				std::unordered_set<int> PlayerList;
				player->GetRefViewList(PlayerList);
				if (player->CASIsHeal(false, true))
					_timerQueue.AddTaskTimer(AttackedId, -1, EV_HEAL, 5000);
				if (remainingHp != 0) {
					player->SendHpChangePacket(AttackedId, objects[AttackedId]->GetHp());
					for (auto id : PlayerList) {
						Session* Players = reinterpret_cast<Session*>(objects[id]);
						if (!Players->GetIsNpc())
							Players->SendHpChangePacket(AttackedId, objects[AttackedId]->GetHp());
					}
				}
				else {
					//player->send_hp_change_packet(AttackedId, objects[AttackedId]->GetHp());
					while (true) {
						Pos pos;
						pos.x = rand() % W_WIDTH / 40;
						pos.y = rand() % W_HEIGHT / 40;
						if (CanGo(pos.x, pos.y)) {
							player->SetExp(player->GetExp() / 2);
							player->SendExpChangePacket();
							player->SetSpawnPos({ pos.x, pos.y });
							break;
						}
					}

					for (auto id : PlayerList) {
						//std::cout << "플레이어 죽음" << std::endl;
						player->SendRemovePlayerPacket(id);
						Session* Players = reinterpret_cast<Session*>(objects[id]);
						if (!Players->GetIsNpc()) {
							Players->SendRemovePlayerPacket(AttackedId);
						}
					}
					_timerQueue.AddTaskTimer(AttackedId, -1, EV_RESPAWN, 3000);
				}

			}
			delete ex_over;
			break;
		}
		case OP_RESPAWN:
			if (objects[key]->GetIsNpc()) {
				SpawnMonster(key);
			}
			else {
				Session* player = reinterpret_cast<Session*>(objects[key]);
				Pos pos = player->GetSpawnPos();;
				player->SetPosition(pos.x, pos.y);
				player->SetHp(player->GetMaxHp());
				SetSectorId(*objects[key], key, pos.x, pos.y);

				int sectorId = (pos.x / SECTOR_SIZE) + ((pos.y / SECTOR_SIZE) * MULTIPLY_ROW);
				sectors[sectorId].AddPlayerList(key);

				player->SendRespawnPacket();

				SpawnPlayer(key, sectorId);
				
			}
			delete ex_over;
			break;
		case OP_HEAL:
			if (true == objects[key]->Heal())
				_timerQueue.AddTaskTimer(key, -1, EV_HEAL, 5000);
			else
				objects[key]->SetHeal(false);

			Session* player = reinterpret_cast<Session*>(objects[key]);
			int hp = player->GetHp();
			player->SendHpChangePacket(key, hp);

			std::unordered_set<int> playerList;
			player->GetRefViewList(playerList);

			for (int id : playerList) {
				if (!objects[id]->GetIsNpc()) {
					reinterpret_cast<Session*>(objects[id])->SendHpChangePacket(key, hp);
				}
			}
			delete ex_over;
			break;
		}
	}
}

void Server::disconnect(int key)
{
	Session* logoutPlayer = reinterpret_cast<Session*>(objects[key]);

	int sectorId = logoutPlayer->GetSectorId();
	sectors[sectorId].RemovePlayerList(key);

	std::unordered_set <int> playerList;
	GetNearPlayersList(key, playerList);

	wchar_t sql[70];
	std::wstring wStr = converter.from_bytes(logoutPlayer->GetName());
	Pos pos = logoutPlayer->GetPosition();
	swprintf(sql, 70, L"EXEC logout_user '%s', %d, %d, %d, %d, %d, %d",
		wStr.c_str(), pos.x, pos.y,
		logoutPlayer->GetLevel(), logoutPlayer->GetExp(), logoutPlayer->GetHp(), logoutPlayer->GetPower());
	_dbQueue.addTaskExecDirect(logoutPlayer, sql, EV_LOGOUT);
	logoutPlayer->SetName("\n");

	for (auto& id : playerList) {
		Session* pl = reinterpret_cast<Session*>(objects[id]);
		{
			std::lock_guard<std::mutex> ll(pl->GetStateMutex());
			if (ST_INGAME != pl->GetState()) continue;
		}
		if (pl->GetId() == key) continue;
		pl->SendRemovePlayerPacket(key);
	}
	closesocket(logoutPlayer->GetSocket());

	std::lock_guard<std::mutex> ll(objects[key]->GetStateMutex());
	objects[key]->SetState(ST_FREE);
	logoutPlayer->ClearViewList();
}

void Server::BroadCastChat(int id, char* p)
{
	SC_CHAT_PACKET chatPacket;
	chatPacket.size = sizeof(SC_CHAT_PACKET) - CHAT_SIZE + strlen(p) + 1 + strlen(objects[id]->GetName()) + 1;
	chatPacket.type = static_cast<int>(SC_CHAT);
	memcpy_s(chatPacket.name, NAME_SIZE, objects[id]->GetName(), NAME_SIZE);
	memcpy_s(chatPacket.mess, CHAT_SIZE, p, CHAT_SIZE);

	for (int i = 0; i < MAX_USER; ++i) {
		Session* player = reinterpret_cast<Session*>(objects[i]);
		if (player->GetState() != ST_INGAME) continue;
		player->DoSend(&chatPacket);
	}
}

void Server::processAttackRequest(int id, char* packet)
{
	Session* player = reinterpret_cast<Session*>(objects[id]);
	if (std::chrono::system_clock::now() < player->_last_attak_time + std::chrono::milliseconds(ATTACK_RATE))
		return;
	player->_last_attak_time = std::chrono::system_clock::now();

	int AttackedId = FindAttackedMonster(id);
	if (AttackedId == -1)
		return;
	Monster* target = reinterpret_cast<Monster*>(objects[AttackedId]);

	int damage = objects[id]->GetPower();
	bool isSuccess = false;;
	int remainingHp = target->Damage(damage, isSuccess);
	if (isSuccess) {
		// 공격성공
		std::unordered_set<int> PlayerList;
		GetNearPlayersList(AttackedId, PlayerList);

		if (remainingHp != 0) {
			// 죽은건 아니면 체력을 수정

			for (auto id : PlayerList) {
				Session* Players = reinterpret_cast<Session*>(objects[id]);
				Players->SendHpChangePacket(AttackedId, target->GetHp());
			}
		}
		else {
			// 몬스터 죽음 부활
			_timerQueue.AddTaskTimer(AttackedId, -1, EV_RESPAWN, 30000);
			// 죽인 플레이어의 경험치 설정
			if (objects[id]->SetAddExp(target->GetExpOnDeath())) {
				player->SendLevelChangePacket(id, player->GetLevel(), player->GetExp());

				// 레벨업 뷰리스트에 있는 플레이어에게 보내기
				std::unordered_set<int> nearPlayer;
				player->GetRefViewList(nearPlayer);
				for (auto playerId : nearPlayer) {
					Session* otherPlayer = reinterpret_cast<Session*>(objects[playerId]);
					otherPlayer->SendLevelChangePacket(id, player->GetLevel(), player->GetExp());
				}

			}
			else {
				// 경험치만 올리기
				player->SendExpChangePacket();
			}

			// 모든 플레이어에게 remove보내기
			target->SetPosition(-100, -100);
			for (auto id : PlayerList) {
				Session* Players = reinterpret_cast<Session*>(objects[id]);
				Players->SendRemovePlayerPacket(AttackedId);
			}
		}

	}
}

void Server::processSkillRequest(int id, char* packet)
{
	int findIds[4]{ -1, -1, -1, -1 };
	bool isSuccess = FindASkillMonster(id, findIds);
	if (isSuccess == -1)
		return;

	for (int i = 0; i < 4; ++i) {
		if (findIds[i] == -1)
			continue;

		Session* player = reinterpret_cast<Session*>(objects[id]);

		int damage = objects[id]->GetPower();
		isSuccess = false;;
		int remainingHp = objects[findIds[i]]->Damage(damage, isSuccess);
		if (isSuccess) {
			// 공격성공
			std::unordered_set<int> PlayerList;
			GetNearPlayersList(findIds[i], PlayerList);

			if (remainingHp != 0) {
				// 죽은건 아니면 체력을 수정

				for (auto id : PlayerList) {
					Session* Players = reinterpret_cast<Session*>(objects[id]);
					Players->SendHpChangePacket(findIds[i], objects[findIds[i]]->GetHp());
				}
			}
			else {
				// 몬스터 죽음 부활
				Monster* target = reinterpret_cast<Monster*>(objects[findIds[i]]);

				_timerQueue.AddTaskTimer(findIds[i], -1, EV_RESPAWN, 30000);
				// 죽인 플레이어의 경험치 설정
				if (objects[id]->SetAddExp(target->GetExpOnDeath())) {
					player->SendLevelChangePacket(id, player->GetLevel(), player->GetExp());

					// 레벨업 뷰리스트에 있는 플레이어에게 보내기
					std::unordered_set<int> nearPlayer;
					player->GetRefViewList(nearPlayer);
					for (auto playerId : nearPlayer) {
						Session* otherPlayer = reinterpret_cast<Session*>(objects[playerId]);
						otherPlayer->SendLevelChangePacket(id, player->GetLevel(), player->GetExp());
					}

				}
				else {
					// 경험치만 올리기
					player->SendExpChangePacket();
				}

				// 모든 플레이어에게 remove보내기
				objects[findIds[i]]->SetPosition(-100, -100);
				for (auto id : PlayerList) {
					Session* Players = reinterpret_cast<Session*>(objects[id]);
					Players->SendRemovePlayerPacket(findIds[i]);
				}
			}

		}
	}
}

void Server::SpawnPlayer(int key, int sectorId)
{
	Session* player = reinterpret_cast<Session*>(objects[key]);
	std::unordered_set<int> playerList;
	std::unordered_set<int> monsterList;
	int index = 0;
	
	for (int i = 0; i < NUM_ADJACENT_SECTORS; ++i) {
		index = sectorId - adjacentSectors[i];
		if (index < 0 || index > SECTOR_COUNT - 1)
			continue;

		sectors[index].GetPlayerList(playerList);

		for (auto& pl_id : playerList) {
			Object& cl = *(objects[pl_id]);
			{
				std::lock_guard<std::mutex> ll(cl.GetStateMutex());
				if (ST_INGAME != cl.GetState()) continue;
			}
			if (pl_id == key)
				continue;
			if (true == CanSee(pl_id, key)) {
				if (true == cl.GetIsNpc()) {
					monsterList.insert(pl_id);
				}
				else {
					player->SendAddPlayerPacket(cl, VI_PLAYER);
					reinterpret_cast<Session*>(&cl)->SendAddPlayerPacket(*player, VI_PLAYER);
				}
			}
		}
	}

	// 몬스터 순회//////////////////////////////////////////////////////////////////////////////////////////////////
	for (auto& mId : monsterList) {
		Monster* monster = reinterpret_cast<Monster*>(objects[mId]);

		if (true == CanSee(mId, key)) {
			if (monster->GetIsAgro()) { // 어그로 몬스터 인데 누군가를 따라가고 있지 않다면 Ai 추적을 해야하는지 판단하는 작업 필요
				if (!monster->GetISAIMove()) {
					OVER_EXP* exover = new OVER_EXP;
					exover->_comp_type = OP_CAN_ASTAR; // 이거는 어그로와 공격 그쪽으로 변경
					exover->_cause_player_id = key;
					PostQueuedCompletionStatus(_hiocp, 1, mId, &exover->_over);
				}
			}

			player->SendAddPlayerPacket(*(objects[mId]), monster->GetMonsterType());

			if (false == monster->GetIsActive()) {
				if (true == monster->CASIsActive(false, true)) {
					// 타이머가 호출되고 로밍 몬스터의 경우 영역에서 움직이는거를 루아가 계산한다. 
					if (monster->GetIsRoaming())
						_timerQueue.AddTaskTimer(mId, -1, EV_RANDOM_MOVE, 1000);
				}
				continue;
			}
			else {
				player->SendAddPlayerPacket(*(objects[mId]), monster->GetMonsterType());
			}
		}
	}
}

void Server::SpawnMonster(int key)
{
	Monster* monster = reinterpret_cast<Monster*>(objects[key]);
	Pos pos = objects[key]->GetSpawnPos();
	monster->SetPosition(pos.x, pos.y);
	monster->SetHp(monster->GetMaxHp());
	SetSectorId(*objects[key], key, pos.x, pos.y);

	std::unordered_set<int> playerList;
	GetNearPlayersList(key, playerList);

	for (auto playerId : playerList) {
		Session* player = reinterpret_cast<Session*>(objects[playerId]);
		player->SendAddPlayerPacket(*objects[key], monster->GetMonsterType());
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
	if (!CanGo(x, y)) {
		return;
	}
	//std::cout << "CS_MOVE " << x << y << "\n";
	movePlayer->SetPosition(x, y);
	movePlayer->SetDir(direction);

	sectors;
	int sectorId = SetSectorId(*movePlayer, id, x, y);

	std::unordered_set<int> oldViewList;
	movePlayer->GetRefViewList(oldViewList);
	std::unordered_set<int> newViewList;

	std::unordered_set<int> playerList;
	int index = 0;
	for (int i = 0; i < NUM_ADJACENT_SECTORS; ++i)
	{
		index = sectorId + adjacentSectors[i];
		if (index < 0 || index > SECTOR_COUNT - 1)
			continue;

		sectors[index].GetPlayerList(playerList);

		for (auto& otherId : playerList) {
			Object& object = *(objects[otherId]);

			if (object.GetState() != ST_INGAME) continue;
			if (otherId == id) continue;
			if (true == CanSee(otherId, id)) {
				newViewList.insert(otherId);
				if ((true == object.GetIsNpc())) {
					Monster* monster = reinterpret_cast<Monster*>(objects[otherId]);

					// 어그로 몬스터의 레이더 검사
					if (monster->GetIsAgro()) {
						if (!monster->GetISAIMove()) {
							OVER_EXP* exover = new OVER_EXP;
							exover->_comp_type = OP_CAN_ASTAR;
							exover->_cause_player_id = id;
							PostQueuedCompletionStatus(_hiocp, 1, otherId, &exover->_over);
						}
					}

					// 로밍 몬스터의 랜덤이동
					if (false == monster->GetIsActive()) {
						bool input = false;
						if (true == monster->CASIsActive(false, true))
							if (monster->GetIsRoaming())
								_timerQueue.AddTaskTimer(otherId, -1, EV_RANDOM_MOVE, 1000);
					}
				}
			}
		}
	}

	movePlayer->SendMovePacket(*(objects[id]), direction);

	for (auto& cl : newViewList) {
		Session* addPlayer = reinterpret_cast<Session*>(objects[cl]);
		char visual = VI_PLAYER;
		if (0 == oldViewList.count(cl)) {
			if (false == addPlayer->GetIsNpc()) {
				addPlayer->SendAddPlayerPacket(*(objects[id]), VI_PLAYER);
			}
			else {
				Monster* monster = reinterpret_cast<Monster*>(objects[cl]);
				visual = monster->GetMonsterType();
			}

			movePlayer->SendAddPlayerPacket(*(objects[cl]), visual);
		}
		else {
			if (false == addPlayer->GetIsNpc()) {
				addPlayer->SendMovePacket(*(objects[id]), direction);
			}
		}
	}

	for (auto& cl : oldViewList) {
		if (0 == newViewList.count(cl)) {
			if (false == objects[cl]->GetIsNpc()) {
				Session* removePlayer = reinterpret_cast<Session*>(objects[cl]);
				removePlayer->SendRemovePlayerPacket(id);
			}
			movePlayer->SendRemovePlayerPacket(cl);
		}
	}
}

void Server::GetNearPlayersList(int id, std::unordered_set<int>& list)
{
	std::unordered_set<int> playerList;
	int SectorId = objects[id]->GetSectorId();
	for (int i = 0; i < NUM_ADJACENT_SECTORS; ++i)
	{
		int index = SectorId - adjacentSectors[i];
		if (index < 0 || index > SECTOR_COUNT - 1)
			continue;

		sectors[index].GetPlayerList(playerList);

		for (auto& p : playerList) {
			Object* s = reinterpret_cast<Session*>(objects[p]);
			if (s->GetIsNpc()) continue;
			if (s->GetState() != ST_INGAME) continue;
			if (CanSee(s->GetId(), id))
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

bool Server::processLoginRequest(int id, char* packet)
{
	CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet);
	Session* loginPlayer = reinterpret_cast<Session*>(objects[id]);

	for (auto player : objects) {
		char* name = player->GetName();
		if (strncmp(p->name, name, strlen(p->name) + 1) == 0) {
			loginPlayer->SendLoginFailPacket();
			return false;
		}
	}

	objects[id]->SetName(p->name);
	wchar_t sqlQuery[100];
	std::wstring convertedName = converter.from_bytes(p->name);
	swprintf(sqlQuery, 100, L"EXEC select_user '%s'", convertedName.c_str());
	_dbQueue.addTaskExecDirect(reinterpret_cast<Session*>(objects[id]), sqlQuery, EV_LOGIN);

	return true;
}

void Server::processMoveRequest(int id, char* packet)
{
	CS_MOVE_PACKET* p = reinterpret_cast<CS_MOVE_PACKET*>(packet);

	Session* movePlayer = reinterpret_cast<Session*>(objects[id]);
	if (std::chrono::system_clock::now() < movePlayer->_last_move_time + std::chrono::milliseconds(MOVE_RATE))
		return;

	movePlayer->_last_move_time = std::chrono::system_clock::now();

	movePlayer->_last_move_time_stress_test = p->moveTime;
	process_move(movePlayer, id, p->direction);
}

void Server::AStar(int& x, int& y, int id)
{
	//std::cout << "Astar\n";

	bool pathSuccess = false;
	static int DIR_COUNT = 4;
	static int cost[] = { 10, 10, 10, 10 };
	static Pos moveDir[] = { Pos{-1, 0}, Pos{0, -1}, Pos{1, 0}, Pos{0, 1} };
	const int size = 11;

	Monster* monster = reinterpret_cast<Monster*>(objects[id]);
	Pos startPos = monster->GetPosition();
	int target = monster->GerTarget();
	if (target == -1) {
		x = startPos.x;
		y = startPos.y;
		//std::cout << "-1이다\n";
		return;
	}
	Pos destPos = objects[target]->GetPosition();

	if (startPos == destPos) {
		while (true) {
			x = startPos.x;
			y = startPos.y;
			OVER_EXP* exover = new OVER_EXP;
			exover->_comp_type = OP_NPC_ATTACK;
			exover->_cause_player_id = target;
			PostQueuedCompletionStatus(_hiocp, 1, id, &exover->_over);
			break;
		}
		monster->SetPosition(x, y);
		return;
	}

	// 최종 길을 저장
	std::vector<Pos> path;
	std::vector<std::vector<bool>> closed(size, std::vector<bool>(size, false));
	// 각 노드에서 현재 가장 좋은 값
	std::vector<std::vector<int>> best(size, std::vector<int>(size, INT32_MAX));
	// 해당 노드를 발견한 노드
	std::map<Pos, Pos> parent;
	// 우선순위 큐, 현재 가장 좋은 노드를 선택하기 위한 노드
	std::priority_queue<PQNode, std::vector<PQNode>, std::greater<PQNode>> pq;

	// 시작 위치를 큐에 넣는다. 
	// F = G + H // G : 이동횟수, H : 목적지까지 예상 값
	Pos indexOffset{ -startPos.x + (size / 2), -startPos.y + (size / 2) };
	int g = 0;
	int h = 10 * (std::abs(destPos.y - startPos.y) + std::abs(destPos.x - startPos.x));
	pq.push(PQNode{ g + h, g, startPos });
	best[startPos.y + indexOffset.y][startPos.x + indexOffset.x] = g + h;
	parent[startPos] = startPos;
	parent[destPos] = destPos;

	while (pq.empty() == false)
	{
		PQNode node = pq.top();
		pq.pop();

		if (closed[node.pos.y + indexOffset.y][node.pos.x + indexOffset.x])
			continue;

		closed[node.pos.y + indexOffset.y][node.pos.x + indexOffset.x] = true;

		// 목적지면 종료
		if (node.pos == destPos) {
			pathSuccess = true;
			break;
		}

		// 애당 위치에서 4방향의 노드 넣기
		for (int dir = 0; dir < DIR_COUNT; ++dir)
		{
			Pos nextPos = node.pos + moveDir[dir];
			// 맵을 나가나?
			if (nextPos.x < 0 || nextPos.y >= W_WIDTH)
				continue;
			if (nextPos.y < 0 || nextPos.x >= W_HEIGHT)
				continue;
			// 몬스터의 위치에서 A*영역안에 있나?
			if (nextPos.x + indexOffset.x < 0 || nextPos.x + indexOffset.x >= size)
				continue;
			if (nextPos.y + indexOffset.y < 0 || nextPos.y + indexOffset.y >= size)
				continue;

			if (CanGo(nextPos.x, nextPos.y) == false)
				continue;

			int g = node.g + cost[dir];
			int h = 10 * (std::abs(destPos.y - nextPos.y) + std::abs(destPos.x - nextPos.x));

			if (best[nextPos.y + indexOffset.y][nextPos.x + indexOffset.x] <= g + h)
				continue;


			best[nextPos.y + indexOffset.y][nextPos.x + indexOffset.x] = g + h;
			pq.push(PQNode{ g + h, g, nextPos });
			parent[nextPos] = node.pos;
		}
	}

	// 길을 못 찾으면 그 자리에서 안 움직이게
	if (pathSuccess == false) {
		x = startPos.x;
		y = startPos.y;
		//Server::GetInstance()->GetTImer()->add_timer(id, EV_RANDOM_MOVE, 1000);
		//std::cout << "길 못 찾음..\n";
		return;
	}
	Pos pos = destPos;

	path.clear();

	while (true)
	{
		path.push_back(pos);

		if (pos == parent[pos])
			break;
		pos = parent[pos];
	}

	reverse(path.begin(), path.end());

	x = path[1].x;
	y = path[1].y;

	//Server::GetInstance()->GetTImer()->add_timer(id, EV_RANDOM_MOVE, 1000);
	monster->SetPosition(x, y);

	if (x == startPos.x) {
		if (y - startPos.y == -1)
			objects[id]->SetDir(DIR_UP);
		else
			objects[id]->SetDir(DIR_DOWN);
	}
	else {
		if (x - startPos.x == -1)
			objects[id]->SetDir(DIR_LEFT);
		else
			objects[id]->SetDir(DIR_RIGHT);
	}
}

int Server::FindAttackedMonster(int id)
{
	static Pos moveDir[] = { Pos{0, -1}, Pos{0, 1}, Pos{-1, 0}, Pos{1, 0} };
	// 뷰리스트에서 몬스터 찾기
	Session* player = reinterpret_cast<Session*>(objects[id]);
	std::unordered_set<int> viewList;
	player->GetRefViewList(viewList);
	Pos playerPos = player->GetPosition();
	int dir = player->GetDir();

	for (auto objectId : viewList) {
		if (!objects[objectId]->GetIsNpc())
			continue;
		Pos pos2 = objects[objectId]->GetPosition();

		if (playerPos + moveDir[dir] == pos2) {
			return objectId;
		}
	}
	return -1;
}

bool Server::FindASkillMonster(int id, int* ids)
{
	static Pos moveDir[] = { Pos{0, -1}, Pos{0, 1}, Pos{-1, 0}, Pos{1, 0} };
	// 뷰리스트에서 몬스터 찾기
	Session* player = reinterpret_cast<Session*>(objects[id]);
	std::unordered_set<int> viewList;
	player->GetRefViewList(viewList);
	Pos playerPos = player->GetPosition();
	int dir = player->GetDir();

	bool isFInd = false;

	for (auto objectId : viewList) {
		if (!objects[objectId]->GetIsNpc())
			continue;
		Pos pos2 = objects[objectId]->GetPosition();

		if (playerPos + moveDir[DIR_UP] == pos2) {
			ids[DIR_UP] = objectId;
			isFInd = true;
		}
		else if (playerPos + moveDir[DIR_DOWN] == pos2) {
			ids[DIR_DOWN] = objectId;
			isFInd = true;
		}
		else if (playerPos + moveDir[DIR_LEFT] == pos2) {
			ids[DIR_LEFT] = objectId;
			isFInd = true;
		}
		else if (playerPos + moveDir[DIR_RIGHT] == pos2) {
			ids[DIR_RIGHT] = objectId;
			isFInd = true;
		}
	}
	if (isFInd)
		return true;
	return false;
}

int Server::GetNewClientId()
{
	for (int i = 0; i < MAX_USER; ++i) {
		std::lock_guard<std::mutex> ll{ objects[i]->GetStateMutex() };
		if (objects[i]->GetState() == ST_FREE) {
			return i;
		}
	}
	return -1;
}