// Thread pool
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <semaphore>
#include <unordered_map>
#include <functional>

// #include "http_ReqRes.h"
#include "http_parser.h"

using HttpRequestHandler_t = std::function<MyHttp::HttpResponse(const MyHttp::HttpRequest&)>;

namespace MyHttp {

class ThreadPool {

private:
	std::unordered_map<std::string, std::unordered_map<RequestType, HttpRequestHandler_t>> handler_map;
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
    ThreadPool(const ThreadPool &) = delete;
    ThreadPool(ThreadPool &&) = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;
    ThreadPool &operator=(ThreadPool &&) = delete;

	ThreadPool();
	ThreadPool(int threads, int queue_size);
	~ThreadPool();
	void Init(int threads, int queue_size);
	void RegisterHandler(RequestType r_type, std::string path, HttpRequestHandler_t handle_func);
	HttpRequestHandler_t GetHandler(const HttpRequest& request);
	int AppendTask(int fd);


};

} // MyHttp