#pragma once
#include "protocol.h"

class RecvBuffer
{
private:
	char _buff[BUF_SIZE];
	int _recvBuff_front;
	int _recvBuff_rear;
	int	_prev_remain;

public:
	RecvBuffer();

	char* GetBuff() { return _buff; }
	char* GetBuff(int front) { return &_buff[front]; }
	int GetRecvBuffFrontIndex() { return _recvBuff_front; }
	void SetRecvBuffForntIndex(int front) { _recvBuff_front = front; }
	void SetAddRecvBuffForntIndex(int front) { _recvBuff_front += front; }
	int GetRecvBuffRearIndex() { return _recvBuff_rear; }
	void SetRecvBuffRearIndex(int rear) { _recvBuff_rear = rear; }
	void SetAddRecvBuffRearIndex(int rear) { _recvBuff_rear += rear; }
	int GetPrevRemain() { return _prev_remain; }
	void SetPrevRemain(int prev) { _prev_remain = prev; }
	int GetBuffFreeSpace() { return BUF_SIZE - _recvBuff_rear; }
};

