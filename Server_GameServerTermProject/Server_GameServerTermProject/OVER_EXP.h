#pragma once
#include "protocol.h"
#include "Defines.h"
#include <WS2tcpip.h>
#include <MSWSock.h>
#include "RecvBuffer.h"

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")
class OVER_EXP 
{
public:
	WSAOVERLAPPED _over;
	RecvBuffer _buff;
	WSABUF _wsabuf;
	//char _send_buf[BUF_SIZE];
	COMP_TYPE _comp_type;
	int _cause_player_id;
	OVER_EXP()
	{
		_wsabuf.len = BUF_SIZE;
		_wsabuf.buf = _buff.GetBuff();
		_comp_type = OP_RECV;
		ZeroMemory(&_over, sizeof(_over));
	}
	OVER_EXP(char* packet)
	{
		_wsabuf.len = *reinterpret_cast<unsigned short*>(packet);
		_wsabuf.buf = _buff.GetBuff();
		ZeroMemory(&_over, sizeof(_over));
		_comp_type = OP_SEND;
		memcpy(_buff.GetBuff(), packet, _wsabuf.len);
	}
};

