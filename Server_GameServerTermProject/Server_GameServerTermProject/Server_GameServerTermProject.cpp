﻿#include <iostream>
#include "Server.h"
#include <vector>
#include <thread>
#include "Timer.h"
#include "DB.h"


int main()
{
	std::wcout.imbue(std::locale("korean"));
	Server* server = Server::GetInstance();
	server->Init();

	Timer* timer = server->GetTImer();
	std::thread timerThread{ [&timer]() {timer->startTimerThread(); } };

	DB* db = server->GetDB();
	std::thread dbThread{ [&db]() {db->startDbThread(); } };

	std::vector<std::thread> worker_threads;
	int num_threads = std::thread::hardware_concurrency();
	for (int i = 0; i < num_threads; ++i)
		worker_threads.emplace_back([&server]() { server->WorkerThread(); });
	for (auto& th : worker_threads)
		th.join();
	timerThread.join();
	dbThread.join();
	delete server;
}
