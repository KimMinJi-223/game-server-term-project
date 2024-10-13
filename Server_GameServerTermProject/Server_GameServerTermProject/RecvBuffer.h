#pragma once
#include "protocol.h"

class RecvBuffer
{
private:
	char _buff[BUF_SIZE]; // Recv���� ����
	int _front;			  // �����͸� Recv�� _buff ��ġ
	int _rear;			  // �����͸� ó���� _buff ��ġ
	int	_prevRemain;	  // _buff�� �ִ� ���� ó���� �������� ũ��

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

