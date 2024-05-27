#include "Sector.h"
#include <atomic>
#include <iostream>

Sector::Sector()
{
}

bool Sector::ReadLock()
{
	int expected;
	int update;

	while (true) {
		expected = _flag & 0x0000FFFF;
		update = expected + 1;
		if (std::atomic_compare_exchange_strong(reinterpret_cast<std::atomic_int*>(&_flag), &expected, update))
		{
			break;
		}
	}

	return true;
}

bool Sector::ReadUnLock()
{
	int expected;
	int update;

	while (true) {
		expected = _flag & 0x0000FFFF;
		update = expected - 1;
		if (std::atomic_compare_exchange_strong(reinterpret_cast<std::atomic_int*>(&_flag), &expected, update))
		{
			break;
		}
	}

	return true;
}

bool Sector::WriteLock()
{
	int expected;
	int update;

	while (true) {
		expected = 0x00000000;
		update = 0x00010000;
		if (std::atomic_compare_exchange_strong(reinterpret_cast<std::atomic_int*>(&_flag), &expected, update))
		{
			break;
		}
	}

	return true;
}

bool Sector::WriteUnLock()
{
	int expected;
	int update;

	while (true) {
		expected = 0x00010000;
		update = 0x00000000;
		if (std::atomic_compare_exchange_strong(reinterpret_cast<std::atomic_int*>(&_flag), &expected, update))
		{
			break;
		}
	}

	return true;
}

void Sector::AddPlayerList(int id)
{
	WriteLock();
	_player_list.insert(id);
	WriteUnLock();
}

void Sector::RemovePlayerList(int id)
{
	WriteLock();
	_player_list.erase(id);
	WriteUnLock();
}
