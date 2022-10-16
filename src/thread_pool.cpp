#include <unistd.h>
#include <iostream>
#include <mutex>

#include "thread_pool.h"
// #include "http_parser.h"

namespace MyHttp{

void ThreadPool::Worker(ThreadPool* parent_pool) {
	std::vector<char> buffer(30000);

    while(!parent_pool->termination_flag) {
        int newfd = parent_pool->GetTask();
        if (newfd == -1) {
        	parent_pool->task_avail.acquire();
        } else {
        	std::fill(buffer.begin(), buffer.end(), 0);
			read(newfd, buffer.data(), 30000);
			std::string s(buffer.begin(), buffer.end());

			auto request = parse(s);
			auto handle_func = parent_pool->GetHandler(request);
			auto resp = handle_func(request);
			std::string string_resp = resp.to_string();

			write(newfd, string_resp.c_str(), string_resp.length());
			close(newfd);
			std::cout << "task done!\n";
        }
    }
    std::cout << "exited\n";
    exit(0);
}

ThreadPool::ThreadPool() {
	this->Init(0,10);
	termination_flag = false;
}

ThreadPool::ThreadPool(int threads, int queue_size) {
	this->Init(threads, queue_size);
}

ThreadPool::~ThreadPool() {
	termination_flag = true;
	for (int i = 0; i < workers_threads.size(); i++) {
		task_avail.release();
	}
	for (int i = 0; i < workers_threads.size(); i++) {
		workers_threads[i].join();
	}
	while(!task_buffer.empty()) {
		int t = task_buffer.front();
		close(t);
		task_buffer.pop();
	}
}

void ThreadPool::Init(int threads, int queue_size) {
	termination_flag = false;
	thread_count = threads;
	max_buffers_size = queue_size;
	workers_threads.resize(threads);
	for (int i = 0; i < threads; i++) {
		std::cout << "created worker\n";
		workers_threads[i] = std::thread(this->Worker, this);
	}
}

void ThreadPool::RegisterHandler(RequestType r_type, std::string path,
		HttpRequestHandler_t handle_func) {
	handler_map[path][r_type] = handle_func;
}

HttpRequestHandler_t ThreadPool::GetHandler(const HttpRequest& request) {
	if (this->handler_map.find(request.get_request_path()) == this->handler_map.end() 
		|| this->handler_map[request.get_request_path()].find(request.get_request_type()) == this->handler_map[request.get_request_path()].end()) {
		return this->handler_map["/error"][RequestType::GET];
	}

	return this->handler_map[request.get_request_path()][request.get_request_type()];
}

int ThreadPool::AppendTask(int fd) {
	std::lock_guard<std::mutex> guard(buffer_lock);
	bool is_empty = task_buffer.empty();
	if (task_buffer.size() < max_buffers_size) {
		task_buffer.push(fd);
		if (is_empty)
			task_avail.release();
		return 0;
	}
	std::cout << "Overflow! Dropping request!\n";
	return 1;
}

int ThreadPool::GetTask() {
	std::lock_guard<std::mutex> guard(buffer_lock);
	if (task_buffer.empty()) {
		return -1;
	}
	int next_task = task_buffer.front();
	task_buffer.pop();
	return next_task;

}


}