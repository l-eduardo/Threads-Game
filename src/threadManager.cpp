#include <iostream>
#include <ncurses.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <unordered_map>
#include <functional>
#include <vector>
#include "threadManager.h"

ThreadManager::ThreadManager(int threadCount) {
		for (int i = 0; i < threadCount; ++i) {
				threads.push_back(std::thread(&ThreadManager::worker, this));
		}
}

ThreadManager::~ThreadManager() {
		// Finalizar as threads
		{
				std::lock_guard<std::mutex> lock(queueMutex);
				stopThreads = true;
		}
		cv.notify_all();
		for (auto &t : threads) {
				if (t.joinable()) {
						t.join();
				}
		}
}

void ThreadManager::startAction(const std::string& key, std::function<void()> action) {
		std::lock_guard<std::mutex> lock(queueMutex);
		if (taskMap.find(key) == taskMap.end()) {
				taskMap[key] = action;
				taskQueue.push(key);
				cv.notify_one();
		} else {
				// std::cout << "Ação '" << key << "' já está em fila ou em execução.\n";
		}
}

void ThreadManager::worker() {
		while (true) {
				std::string taskKey;
				{
						std::unique_lock<std::mutex> lock(queueMutex);
						cv.wait(lock, [this]() { return !taskQueue.empty() || stopThreads; });
						
						if (stopThreads && taskQueue.empty()) {
								return;
						}

						taskKey = taskQueue.front();
						taskQueue.pop();
				}

				std::function<void()> task;
				{
						std::lock_guard<std::mutex> lock(taskMutex);
						if (taskMap.find(taskKey) != taskMap.end()) {
								task = taskMap[taskKey];
						}
				}

				if (task) {
						// std::cout << "Thread " << std::this_thread::get_id() << " executando ação '" << taskKey << "'\n";
						task();
						{
								std::lock_guard<std::mutex> lock(taskMutex);
								taskMap.erase(taskKey);
						}
						// std::cout << "Ação '" << taskKey << "' concluída.\n";
				}
		}
}
