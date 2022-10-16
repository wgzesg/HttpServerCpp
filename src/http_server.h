// My http server class
#pragma once
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <unistd.h> 
#include <string.h>
#include <iostream>
#include <vector>

// #include "http_parser.h"
#include "thread_pool.h"

using HttpRequestHandler_t = std::function<MyHttp::HttpResponse(const MyHttp::HttpRequest&)>;
namespace MyHttp {

class HttpServer {

private:
	int sockfd;
	std::vector<epoll_event> events;
	int ep_instance;
	ThreadPool thread_pool;

	HttpRequestHandler_t get_handler_func(const HttpRequest& request);
	int send_response(const HttpResponse& resp);
	void HandleRequest();
	int AddEpollEvent(int new_fd, int flag);

public:
	HttpServer();
	~HttpServer();

    HttpServer(const HttpServer &) = delete;
    HttpServer(HttpServer &&) = delete;
    HttpServer &operator=(const HttpServer &) = delete;
    HttpServer &operator=(HttpServer &&) = delete;

	void Listen(int port, std::string ip_address, int max_buffer=50);
	void Run();
	void Close();

	void RegisterHandler(RequestType r_type, std::string path, HttpRequestHandler_t handle_func);
};
}