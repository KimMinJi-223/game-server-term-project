#include "Session.h"

void Session::Init(int x, int y, int id, const char* name, SOCKET socket)
{
	Object::Init(x, y, id, name);
	_prev_remain = 0;
	_socket = socket;
	_is_moving = false;
}

void Session::do_recv()
{
	DWORD recv_flag = 0;
	memset(&_recv_over._over, 0, sizeof(_recv_over._over));
	_recv_over._wsabuf.len = BUF_SIZE - _prev_remain;
	_recv_over._wsabuf.buf = _recv_over._send_buf + _prev_remain;
	WSARecv(_socket, &_recv_over._wsabuf, 1, 0, &recv_flag,
		&_recv_over._over, 0);
}

void Session::do_send(void* packet)
{
	OVER_EXP* sdata = new OVER_EXP{ reinterpret_cast<char*>(packet) };
	WSASend(_socket, &sdata->_wsabuf, 1, 0, 0, &sdata->_over, 0);
}

void Session::send_login_info_packet(OBJECT_VISUAL visual)
{
	SC_LOGIN_INFO_PACKET p;
	p.id = _id;
	p.size = sizeof(SC_LOGIN_INFO_PACKET);
	p.type = static_cast<char>(SC_PACKET_ID::SC_LOGIN_INFO);
	p.visual = visual;
	p.x = _pos.x;
	p.y = _pos.y;
	do_send(&p);
}

void Session::send_add_player_packet(Object& other, char c_visual)
{
	printf("send_add_player_packet %d -> %d (%d, %d)\n", _id, other.GetId(), other.GetPosition().x, other.GetPosition().y);
	int c_id = other.GetId();
	_vll.lock();
	if (0 != _view_list.count(c_id)) {
		_vll.unlock();
		return;
	}
	_view_list.insert(c_id);
	_vll.unlock();

	SC_ADD_PLAYER_PACKET add_packet;
	add_packet.id = c_id;
	add_packet.visual = c_visual;
	strcpy_s(add_packet.name, other.GetName());
	add_packet.size = sizeof(add_packet);
	add_packet.type = static_cast<int>(SC_PACKET_ID::SC_ADD_PLAYER);

	Pos pos = other.GetPosition();
	add_packet.x = pos.x;
	add_packet.y = pos.y;
	do_send(&add_packet);
}

void Session::send_move_packet(Object& other, char dir)
{
	printf("send_move_packet %d \n", other.GetId());

	SC_MOVE_PLAYER_PACKET p;
	p.id = other.GetId();
	p.size = sizeof(SC_MOVE_PLAYER_PACKET);
	p.type = static_cast<int>(SC_PACKET_ID::SC_MOVE_PLAYER);

	Pos pos = other.GetPosition();
	p.x = pos.x;
	p.y = pos.y;
	p.direction = dir;
	p.move_time = static_cast<Session*>(&other)->_last_move_time;
	do_send(&p);
}

void Session::send_remove_player_packet(int c_id)
{
	_vll.lock();
	if (0 == _view_list.count(c_id)) {
		_vll.unlock();
		return;
	}
	_view_list.erase(c_id);
	_vll.unlock();

	SC_REMOVE_PLAYER_PACKET p;
	p.id = c_id;
	p.size = sizeof(p);
	p.type = static_cast<int>(SC_PACKET_ID::SC_REMOVE_PLAYER);
	do_send(&p);
}

void Session::GetRefViewList(std::unordered_set<int>& view)
{
	_vll.lock();
	view = _view_list;
	_vll.unlock();

}

