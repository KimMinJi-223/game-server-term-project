#pragma once
constexpr int PORT_NUM = 4000;
constexpr int NAME_SIZE = 20;
constexpr int CHAT_SIZE = 300;

constexpr int MAX_USER = 10000;
constexpr int NUM_NPC = 200;
constexpr int NUM_NPC_1 = MAX_USER + 50;
constexpr int NUM_NPC_2 = NUM_NPC_1 + 50;
constexpr int NUM_NPC_3 = NUM_NPC_2 + 50;
constexpr int NUM_NPC_4 = NUM_NPC_3 + 50;

constexpr int BUF_SIZE = 1024; // �߰�
constexpr int MAX_PACKET_SIZE = 350; // �߰�

constexpr int W_WIDTH = 100;
constexpr int W_HEIGHT = 100;

// Packet ID
constexpr char CS_LOGIN = 0;
constexpr char CS_MOVE = 1;
constexpr char CS_CHAT = 2;
constexpr char CS_ATTACK = 3;			// 4 ���� ����
constexpr char CS_TELEPORT = 4;			// RANDOM�� ��ġ�� Teleport, Stress Test�� �� Hot Spot������ ���ϱ� ���� ����
constexpr char CS_LOGOUT = 5;			// Ŭ���̾�Ʈ���� ���������� ������ �����ϴ� ��Ŷ
constexpr char CS_A_SKILL = 6; // �߰�

constexpr char SC_LOGIN_INFO = 2;
constexpr char SC_LOGIN_FAIL = 3;
constexpr char SC_ADD_OBJECT = 4;
constexpr char SC_REMOVE_OBJECT = 5;
constexpr char SC_MOVE_OBJECT = 6;
constexpr char SC_CHAT = 7;
constexpr char SC_STAT_CHANGE = 8; // ���°� �ٲ�� �� �������Ѵٰ� �Ͻ�


enum OBJECT_VISUAL { VI_AVATAR, VI_PLAYER, VI_MONSTER_1, VI_MONSTER_2, VI_MONSTER_3, VI_MONSTER_4 }; // �߰�

enum Dir // �߰�
{
	DIR_UP,
	DIR_DOWN,
	DIR_LEFT,
	DIR_RIGHT,
};

#pragma pack (push, 1)
struct CS_LOGIN_PACKET {
	unsigned short size;
	char	type;
	char	name[NAME_SIZE];
};

struct CS_MOVE_PACKET {
	unsigned short size;
	char	type;
	char	direction;  // 0 : UP, 1 : DOWN, 2 : LEFT, 3 : RIGHT
	unsigned	move_time;
};


struct CS_CHAT_PACKET {
	unsigned short size;			// ũ�Ⱑ �����̴�, mess�� ������ size�� ������.
	char	type;
	char	mess[CHAT_SIZE];
};

struct CS_ATTACK_PACKET { // �߰�
	unsigned short size;
	char	type;
};

struct CS_A_ATTACK_PACKET { // �߰�
	unsigned short size;
	char	type;
};

struct CS_A_SKILL_PACKET {
	unsigned short size;
	char	type;
};

struct CS_TELEPORT_PACKET {			// �������� �ڷ���Ʈ �ϴ� ��Ŷ, ���� �׽�Ʈ�� �ʿ�
	unsigned short size;
	char	type;
};

struct CS_LOGOUT_PACKET {
	unsigned short size;
	char	type;
};

struct SC_LOGIN_INFO_PACKET {
	unsigned short size;
	char	type;
	int		visual;				// ����, �������� ������ �� ���
	int		id;
	int		hp;
	int		max_hp;
	int		exp;
	int		level;
	short	x, y;
};

struct SC_ADD_OBJECT_PACKET {
	unsigned short size;
	char	type;
	int		id;
	int		visual;				// ��� ���� OBJECT�ΰ��� ����
	short	x, y;
	char	name[NAME_SIZE];
};

struct SC_REMOVE_OBJECT_PACKET {
	unsigned short size;
	char	type;
	int		id;
};


struct SC_MOVE_OBJECT_PACKET {
	unsigned short size;
	char	type;
	int		id;
	short	x, y;
	char	direction;  // 0 : UP, 1 : DOWN, 2 : LEFT, 3 : RIGHT
	unsigned int move_time;
};

struct SC_CHAT_PACKET {
	unsigned short size;
	char	type;
	int		id;
	char	mess[CHAT_SIZE];
};

struct SC_LOGIN_FAIL_PACKET {
	unsigned short size;
	char	type;
};

struct SC_STAT_CHANGE_PACKET {
	unsigned short size;
	char	type;
	int		hp;
	int		max_hp;
	int		exp;
	int		level;
};


#pragma pack (pop)