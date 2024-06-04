#pragma once
constexpr int PORT_NUM = 4000;
constexpr int BUF_SIZE = 1024;
constexpr int MAX_USER = 17000;
constexpr int NUM_NPC = 20000;

constexpr int NAME_SIZE = 20;
constexpr int CHAT_SIZE = 100;

constexpr int W_WIDTH = 100;
constexpr int W_HEIGHT = 100;

constexpr int MAX_PACKET_SIZE = 150;

enum OBJECT_VISUAL { VI_AVATAR, VI_PLAYER, VI_NPC };

enum Dir
{
	DIR_UP,
	DIR_DOWN,
	DIR_LEFT,
	DIR_RIGHT,
};

enum class CS_PACKET_ID { 
	CS_LOGIN, 
	CS_MOVE,
	CS_CHAT,
	CS_MOVE_STOP
};

enum class SC_PACKET_ID { 
	SC_LOGIN_INFO, 
	SC_ADD_PLAYER, 
	SC_REMOVE_PLAYER, 
	SC_MOVE_PLAYER, 
	SC_CHAT
};

#pragma pack (push, 1)
struct CS_LOGIN_PACKET {
	unsigned char size;
	char	type;
	char	name[NAME_SIZE];
};

struct CS_MOVE_PACKET {
	unsigned char size;
	char	type;
	char	direction;  // 0 : UP, 1 : DOWN, 2 : LEFT, 3 : RIGHT
	unsigned	move_time;
};

struct CS_CHAT_PACKET {
	unsigned char size;
	char	type;
	char	mess[CHAT_SIZE];
};

struct CS_MOVE_STOP_PACKET {
	unsigned char size;
	char	type;
};

struct SC_LOGIN_INFO_PACKET {
	unsigned char size;
	char	type;
	char	visual;
	int	id;
	short	x, y;
};

struct SC_ADD_PLAYER_PACKET {
	unsigned char size;
	char	type;
	int		id;
	char	visual;
	short	x, y;
	char	name[NAME_SIZE];
};

struct SC_REMOVE_PLAYER_PACKET {
	unsigned char size;
	char	type;
	int		id;
};

struct SC_MOVE_PLAYER_PACKET {
	unsigned char size;
	char	type;
	int		id;
	short	x, y;
	char	direction;
	unsigned int move_time;
};

struct  SC_CHAT_PACKET
{
	unsigned char size;
	char	type;
	char	name[NAME_SIZE];
	char	mess[CHAT_SIZE];
};
#pragma pack (pop)