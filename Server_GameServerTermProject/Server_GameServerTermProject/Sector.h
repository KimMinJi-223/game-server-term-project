#pragma once
#include <unordered_set>
#include <mutex>

class Sector
{
public:
	std::unordered_set<int> _playerList;
	int _flag; // 0x0000(쓰기) 0x0000(읽기)
	std::mutex m; // 성능비교 용

private:
	bool ReadLock();
	bool ReadUnLock();
	bool WriteLock();
	bool WriteUnLock();
public:
	Sector();

	void GetPlayerList(std::unordered_set<int>& list);
	void AddPlayerList(int id);
	void RemovePlayerList(int id);
};

