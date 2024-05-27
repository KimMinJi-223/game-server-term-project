#include "Timer.h"

void Timer::add_timer(int obj_id, EVENT_TYPE et, int ms)
{
	TIMER_EVENT ev;
	ev.obj_id = obj_id;
	ev.event_type = et;
	ev.wakeup_time = std::chrono::system_clock::now() + std::chrono::milliseconds(ms);
	_timer_queue.push(ev);
}
