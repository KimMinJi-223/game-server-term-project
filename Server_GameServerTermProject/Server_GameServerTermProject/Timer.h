#pragma once
#include "Defines.h"
#include <concurrent_priority_queue.h>
#include <chrono>

struct TIMER_EVENT {
	int	id;
	std::chrono::system_clock::time_point wakeupTime;
	EVENT_TYPE eventType;
	int targetId;

	bool operator<(const TIMER_EVENT& e) const
	{
		return wakeupTime > e.wakeupTime;
	}
};

class Timer
{
private:
	concurrency::concurrent_priority_queue<TIMER_EVENT> _timerTaskQueue;
	HANDLE _hiocp;

public:
	void Init(HANDLE hiocp);

public:
	void AddTaskTimer(int id, int targetId, EVENT_TYPE et, int ms);
	void startTimerThread();
};

