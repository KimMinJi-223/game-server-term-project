#pragma once
#include "protocol.h"

class RecvBuffer
{
private:
	char _buff[BUF_SIZE]; // Recv받을 버퍼
	int _front;			  // 데이터를 Recv할 _buff 위치
	int _rear;			  // 데이터를 처리할 _buff 위치
	int	_prevRemain;	  // _buff에 있는 실제 처리할 데이터의 크기

public:
	RecvBuffer();

	char* GetBuff() { return _buff; }
	char* GetBuff(int front) { return &_buff[front]; }
	int GetFrontIndex() { return _front; }
	void SetFrontIndex(int front) { _front = front; }
	void SetAddFrontIndex(int front) { _front += front; }
	int GetRecvBuffRearIndex() { return _rear; }
	void SetRecvBuffRearIndex(int rear) { _rear = rear; }
	void SetAddRecvBuffRearIndex(int rear) { _rear += rear; }
	int GetPrevRemain() { return _prevRemain; }
	void SetPrevRemain(int prev) { _prevRemain = prev; }
	int GetBuffFreeSpace() { return BUF_SIZE - _rear; }
};

