#include "RecvBuffer.h"

RecvBuffer::RecvBuffer()
{
	_front = 0;
	_rear = 0;
	_prevRemain = 0;
}
