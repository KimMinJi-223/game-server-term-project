#include "Timer.h"
#include <thread>
#include "OVER_EXP.h"

void Timer::Init(HANDLE hiocp)
{
	_hiocp = hiocp;
}

void Timer::AddTaskTimer(int id, int targetId, EVENT_TYPE et, int ms)
{
	TIMER_EVENT ev;
	ev.id = id;
	ev.targetId = targetId;
	ev.eventType = et;
	ev.wakeupTime = std::chrono::system_clock::now() + std::chrono::milliseconds(ms);
	_timerTaskQueue.push(ev);
}

void Timer::startTimerThread()
{
	while (true) {
		TIMER_EVENT ev;
		auto current_time = std::chrono::system_clock::now();
		if (_timerTaskQueue.try_pop(ev)) {
			if (ev.wakeupTime > current_time) {
				_timerTaskQueue.push(ev);
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				continue;
			}
			OVER_EXP* ov = new OVER_EXP;
			switch (ev.eventType) {
			case EV_RANDOM_MOVE:
				ov->_comp_type = OP_NPC_MOVE;
				PostQueuedCompletionStatus(_hiocp, 1, ev.id, &ov->_over);
				break;
			case EV_AI_MOVE:
				ov->_comp_type = OP_ASTAR_MOVE;
				ov->_cause_player_id = ev.targetId;
				PostQueuedCompletionStatus(_hiocp, 1, ev.id, &ov->_over);
				break;
			case EV_AI_LUA:
				ov->_comp_type = OP_CAN_ASTAR;
				ov->_cause_player_id = ev.targetId;
				PostQueuedCompletionStatus(_hiocp, 1, ev.id, &ov->_over);
				break;
			case EV_RESPAWN:
				ov->_comp_type = OP_RESPAWN;
				PostQueuedCompletionStatus(_hiocp, 1, ev.id, &ov->_over);
				break;
			case EV_HEAL:
				ov->_comp_type = OP_HEAL;
				PostQueuedCompletionStatus(_hiocp, 1, ev.id, &ov->_over);
				break;
			}

			continue;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}