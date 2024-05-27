#include "pch.h"
#include "NetworkManager.h"
#include "..\..\Server_GameServerTermProject\Server_GameServerTermProject\protocol.h"
void NetworkManager::Init()
{
}

void NetworkManager::Update()
{
}

void NetworkManager::ProcessPacket(char* ptr)
{
	/*static bool first_time = true;
	switch (ptr[1])
	{
	case static_cast<int>(SC_PACKET_ID::SC_LOGIN_INFO):
	{
		SC_LOGIN_INFO_PACKET* packet = reinterpret_cast<SC_LOGIN_INFO_PACKET*>(ptr);
		g_myid = packet->id;
		avatar.m_x = packet->x;
		avatar.m_y = packet->y;
		g_left_x = packet->x - 8;
		g_top_y = packet->y - 8;
		avatar.show();
	}
	break;

	case static_cast<int>(SC_PACKET_ID::SC_ADD_PLAYER):
	{
		SC_ADD_PLAYER_PACKET* my_packet = reinterpret_cast<SC_ADD_PLAYER_PACKET*>(ptr);
		int id = my_packet->id;

		if (id == g_myid) {
			avatar.move(my_packet->x, my_packet->y);
			g_left_x = my_packet->x - 4;
			g_top_y = my_packet->y - 4;
			avatar.show();
		}
		else {
			if (my_packet->visual == VI_PLAYER)
				players[id] = OBJECT{ *pieces, 0, 0, 64, 64 };
			else if (my_packet->visual == VI_NPC)
				players[id] = OBJECT{ *pieces, 64, 0, 64, 64 };

			players[id].move(my_packet->x, my_packet->y);
			players[id].set_name(my_packet->name);
			players[id].show();
		}

		break;
	}
	case static_cast<int>(SC_PACKET_ID::SC_MOVE_PLAYER):
	{
		SC_MOVE_PLAYER_PACKET* my_packet = reinterpret_cast<SC_MOVE_PLAYER_PACKET*>(ptr);
		int other_id = my_packet->id;
		if (other_id == g_myid) {
			avatar.move(my_packet->x, my_packet->y);
			g_left_x = my_packet->x - 8;
			g_top_y = my_packet->y - 8;
		}
		else {
			players[other_id].move(my_packet->x, my_packet->y);
		}		break;
	}

	case static_cast<int>(SC_PACKET_ID::SC_REMOVE_PLAYER):
	{
		SC_REMOVE_PLAYER_PACKET* my_packet = reinterpret_cast<SC_REMOVE_PLAYER_PACKET*>(ptr);
		int other_id = my_packet->id;
		if (other_id == g_myid) {
			avatar.hide();
		}
		else {
			players.erase(other_id);
		}
		break;
	}
	case static_cast<int>(SC_PACKET_ID::SC_CHAT):
	{
		SC_CHAT_PACKET* my_packet = reinterpret_cast<SC_CHAT_PACKET*>(ptr);
		int other_id = my_packet->id;
		if (other_id == g_myid) {
			avatar.set_chat(my_packet->mess);
		}
		else {
			players[other_id].set_chat(my_packet->mess);
		}

		break;
	}
	default:
		printf("Unknown PACKET type [%d]\n", ptr[1]);
	}*/
}
