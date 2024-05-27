#pragma once
#include "Defines.h"
#include <concurrent_priority_queue.h>
#include <chrono>

struct TIMER_EVENT {
	int	obj_id;
	std::chrono::system_clock::time_point wakeup_time;
	EVENT_TYPE event_type;
	int target_obj;

	bool operator<(const TIMER_EVENT& e) const
	{
		return wakeup_time > e.wakeup_time;
	}

};

class Timer
{
private:
	concurrency::concurrent_priority_queue<TIMER_EVENT> _timer_queue;

public:
	void add_timer(int obj_id, EVENT_TYPE et, int ms);


};

