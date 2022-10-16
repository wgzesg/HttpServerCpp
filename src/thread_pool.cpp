#include "thread_pool.h"
#include <unistd.h>
#include <iostream>
#include <mutex>

namespace MyHttp{

void ThreadPool::Worker(ThreadPool* parent_pool) {
	std::vector<char> buffer(30000);

    while(parent_pool->termination_flag) {
        int newfd = parent_pool->GetTask();
        if( !newfd ) {
        	task_avail.acquire();
            // pthread_cond_wait(&(parent_pool->coming_new_tasks));
        } else {
        	std::fill(buffer.begin(), buffer.end(), 0);
			std::cout << "receiving data\n";

			read(newfd, buffer.data(), 30000);
			std::string s(buffer.begin(), buffer.end());

			// auto request = parse(s);

			// auto handle_func = this->get_handler_func(request);

			// auto resp = handle_func(request);

			// std::string string_resp = resp.to_string();

			write(newfd, s.c_str(), s.length());
			close(newfd);
        }
    }
    exit(0);
}

ThreadPool::ThreadPool() {
	ThreadPool(1, 10);
}

ThreadPool::ThreadPool(int threads, int queue_size) {
	this->Init(threads, queue_size);
}

ThreadPool::~ThreadPool() {
	termination_flag = true;
	// pthread_cond_broadcast(&coming_new_tasks);
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
	thread_count = threads;
	max_buffers_size = queue_size;
	workers_threads.resize(threads);
	for (int i = 0; i < threads; i++) {
		workers_threads[i] = std::thread(this->Worker, this);
	}
}

int ThreadPool::AppendTask(int fd) {
	std::lock_guard<std::mutex> guard(buffer_lock);
	if (task_buffer.size() < max_buffers_size) {
		task_buffer.push(fd);
		task_avail.release();
		return 0;
	}
	std::cout << "Overflow! Dropping request!\n";
	return 1;
}

int ThreadPool::GetTask() {
	std::lock_guard<std::mutex> guard(buffer_lock);
	if (task_buffer.empty()) {
		return 0;
	}
	int next_task = task_buffer.front();
	task_buffer.pop();
	return next_task;

}


}