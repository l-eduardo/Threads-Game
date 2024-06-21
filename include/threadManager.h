#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <unordered_map>

class ThreadManager {
public:
  ThreadManager();
  ~ThreadManager();
	
	void startAction(const std::string& key, std::function<void()> action) {
private:
	void worker()
		std::vector<std::thread> threads;
    std::queue<std::string> taskQueue;
    std::unordered_map<std::string, std::function<void()>> taskMap;
    std::mutex queueMutex;
    std::mutex taskMutex;
    std::condition_variable cv;
    bool stopThreads = false;
};
