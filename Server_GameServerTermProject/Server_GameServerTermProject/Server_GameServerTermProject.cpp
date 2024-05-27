#include <iostream>
#include "Server.h"
#include <vector>
#include <thread>

int main()
{
	Server* server = new Server();
	server->Init();

	std::vector<std::thread> worker_threads;
	int num_threads = std::thread::hardware_concurrency();
	for (int i = 0; i < num_threads; ++i)
		worker_threads.emplace_back([&server]() { server->WorkerThread(); });
	for (auto& th : worker_threads)
		th.join();

	delete server;
}
