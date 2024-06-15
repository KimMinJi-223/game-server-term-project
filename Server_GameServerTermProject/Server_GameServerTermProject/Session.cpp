#include "Session.h"

void Session::Init(int x, int y, int id, const char* name, SOCKET socket)
{
	Object::Init(x, y, id, name);
	_socket = socket;
}

void Session::Login(int x, int y, const char* name, int hp, int level, int exp)
{
	_pos.x = x;
	_pos.y = y;
	strcpy_s(_name, NAME_SIZE, name);
	_hp = hp;
	_maxHp = DEFALUT_MAX_HP * level;
	_level = level;
	_exp = exp;
	_power = 100;
	_maxExp = DEFALUT_EXP * pow(2, _level - 1);
}

void Session::do_recv()
{
	DWORD recv_flag = 0;
	memset(&_recv_over._over, 0, sizeof(_recv_over._over));
	_recv_over._wsabuf.len = _recv_over._buff.GetBuffFreeSpace();
	int rear = _recv_over._buff.GetRecvBuffRearIndex();
	_recv_over._wsabuf.buf = _recv_over._buff.GetBuff(rear);
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
	p.size = sizeof(SC_LOGIN_INFO_PACKET);
	p.type = SC_LOGIN_INFO;
	p.visual = visual;
	p.id = _id;

	// DB에서 읽은거 넣어야함
	p.hp = _hp;
	p.max_hp = _maxHp;
	p.exp = _exp;
	p.level = _level;
	p.x = _pos.x;
	p.y = _pos.y;
	///////////////////////
	do_send(&p);
}

void Session::send_add_player_packet(Object& other, char c_visual)
{
	int c_id = other.GetId();
	_vll.lock();
	if (0 != _view_list.count(c_id)) {
		_vll.unlock();
		return;
	}
	_view_list.insert(c_id);
	_vll.unlock();

	//printf("send_add_player_packet %d -> %d (%d, %d)\n", _id, other.GetId(), other.GetPosition().x, other.GetPosition().y);
	SC_ADD_OBJECT_PACKET add_packet;
	add_packet.size = sizeof(SC_ADD_OBJECT_PACKET);
	add_packet.type = SC_ADD_OBJECT;
	add_packet.id = c_id;
	add_packet.visual = c_visual;
	add_packet.hp = other.GetHp();
	add_packet.level = other.GetLevel();;
	memcpy_s(add_packet.name, NAME_SIZE, _name, NAME_SIZE);

	Pos pos = other.GetPosition();
	add_packet.x = pos.x;
	add_packet.y = pos.y;

	strcpy_s(add_packet.name, other.GetName());

	do_send(&add_packet);
}

void Session::send_move_packet(Object& other, char dir)
{
	//printf("%d -> %d send_move_packet\n", other.GetId(), _id);
	SC_MOVE_OBJECT_PACKET p;
	p.size = sizeof(SC_MOVE_OBJECT_PACKET);
	p.type = SC_MOVE_OBJECT;
	p.id = other.GetId();
	p.direction = dir;
	Pos pos = other.GetPosition();
	p.x = pos.x;
	p.y = pos.y;
	//p.direction = dir;
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

	//printf("%d -> %d send_remove_player_packet\n", c_id, _id);
	SC_REMOVE_OBJECT_PACKET p;
	p.size = sizeof(p);
	p.type = SC_REMOVE_OBJECT;
	p.id = c_id;

	do_send(&p);
}

void Session::send_exp_change_packet()
{
	SC_EXP_CHANGE_PACKET p;
	p.size = sizeof(p);
	p.type = SC_EXP_CHANGE;
	p.exp = _exp;

	do_send(&p);
}

void Session::send_hp_change_packet(int id, int hp)
{
	SC_HP_CHANGE_PACKET p;
	p.size = sizeof(SC_HP_CHANGE_PACKET);
	p.type = SC_HP_CHANGE;
	p.id = id;
	p.hp = hp;

	do_send(&p);
}

void Session::send_level_change_packet(int id, int level, int exp)
{
	SC_LEVEL_CHANGE_PACKET p;
	p.size = sizeof(SC_LEVEL_CHANGE_PACKET);
	p.type = SC_LEVEL_CHANGE;
	p.id = id;
	p.level = level;
	p.exp = exp;

	do_send(&p);
}

void Session::GetRefViewList(std::unordered_set<int>& view)
{
	_vll.lock();
	view = _view_list;
	_vll.unlock();

}

