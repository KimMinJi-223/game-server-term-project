#pragma once
#include <unordered_set>
#include <mutex>

class Sector
{
public:
	std::unordered_set<int> _player_list;
	int _flag; // 0000(쓰기) 0000(읽기)
	std::mutex m;
public:
	Sector();

	bool ReadLock();
	bool ReadUnLock();
	bool WriteLock();
	bool WriteUnLock();

	void AddPlayerList(int id);
	void RemovePlayerList(int id);
};

