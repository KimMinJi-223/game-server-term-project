#include "pch.h"
#include "TimeManager.h"

void TimeManager::Init()
{
	::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&_frequency));
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&_prevCount));
}

void TimeManager::Update()
{
	uint64 currentCount;
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCount));

	_deltaTime = (currentCount - _prevCount) / static_cast<float>(_frequency);
	_prevCount = currentCount;

	++_frameCount;
	_frameTime += _deltaTime;

	if (_frameTime >= 1.f)
	{
		// 매 초마다 몇번 갱신했는지가 FPS이다. 
		// 그러기 위해서는 몇초가 지났는지와 몇번 그렸는지가 필요하다.
		_fps = static_cast<uint32>(_frameCount / _frameTime);
		
		_frameTime = 0.f;
		_frameCount = 0;
	}
}
