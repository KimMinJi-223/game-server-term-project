#include "RecvBuffer.h"

RecvBuffer::RecvBuffer()
{
	_recvBuff_front = 0;
	_recvBuff_rear = 0;
	_prev_remain = 0;
}
