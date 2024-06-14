#include "Timer.h"
#include <thread>
#include "OVER_EXP.h"
#include <thread>
using namespace std::chrono_literals;

void Timer::Init(HANDLE hiocp)
{
	_hiocp = hiocp;
}

void Timer::add_timer(int obj_id, int targetId, EVENT_TYPE et, int ms)
{
	TIMER_EVENT ev;
	ev.obj_id = obj_id;
	ev.target_obj = targetId;
	ev.event_type = et;
	ev.wakeup_time = std::chrono::system_clock::now() + std::chrono::milliseconds(ms);
	_timer_queue.push(ev);
}

void Timer::do_timer()
{
	while (true) {
		TIMER_EVENT ev;
		auto current_time = std::chrono::system_clock::now();
		if (_timer_queue.try_pop(ev)) {
			if (ev.wakeup_time > current_time) {
				_timer_queue.push(ev);
				std::this_thread::sleep_for(1ms);
				continue;
			}
			OVER_EXP* ov = new OVER_EXP;
			switch (ev.event_type) {
			case EV_RANDOM_MOVE:
				ov->_comp_type = OP_NPC_MOVE;
				PostQueuedCompletionStatus(_hiocp, 1, ev.obj_id, &ov->_over);
				break;
			case EV_AI_MOVE:
				ov->_comp_type = OP_AI_MOVE;
				ov->_cause_player_id = ev.target_obj;
				PostQueuedCompletionStatus(_hiocp, 1, ev.obj_id, &ov->_over);
				break;
			}
			continue;
		}

		std::this_thread::sleep_for(1ms);
	}
}