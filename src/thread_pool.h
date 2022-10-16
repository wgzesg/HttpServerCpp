// Thread pool
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <semaphore>

namespace MyHttp {

class ThreadPool {

private:
	bool termination_flag = false;
	std::mutex buffer_lock;
	int thread_count;
	int max_buffers_size;
	std::vector<std::thread> workers_threads;
	std::binary_semaphore task_avail{0};
	std::queue<int> task_buffer;
	static void Worker(ThreadPool* parent_pool);
	int GetTask();

public:
	ThreadPool();
	ThreadPool(int threads, int queue_size);
	~ThreadPool();
	void Init(int threads, int queue_size);
	int AppendTask(int fd);

};

} // MyHttp