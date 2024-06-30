#ifndef THREADMANAGER_H
#define THREADMANAGER_H

#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>
#include <queue>
#include <condition_variable>

class ThreadManager {
private:
	void worker();
		std::vector<std::thread> threads;
    std::queue<std::string> taskQueue;
    std::unordered_map<std::string, std::function<void()>> taskMap;
    std::mutex queueMutex;
    std::mutex taskMutex;
    std::condition_variable cv;
    bool stopThreads = false;

public:
  ThreadManager(int threadCount = 4);
  ~ThreadManager();
  
  void startAction(const std::string& key, std::function<void()> action);
};

#endif // THREADMANAGER_H
