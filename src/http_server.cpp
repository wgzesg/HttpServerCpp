#include "http_server.h"
#include "utils.h"
#include <arpa/inet.h>

namespace MyHttp {

HttpServer::HttpServer() {
}

HttpServer::~HttpServer() {
	this->Close();
}

int HttpServer::AddEpollEvent(int new_fd, int flag) {
	epoll_event ev;
	ev.events = flag;
	ev.data.fd = new_fd;
	if (epoll_ctl(this->ep_instance, EPOLL_CTL_ADD, new_fd, &ev) == -1) {
		return 1;
	}
	return 0;
}

void HttpServer::Listen(int port, std::string ip_address, int max_buffer) {
	thread_pool.Init(32, max_buffer);
	this->sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->sockfd < 0) {
		std::cout << "error creating socket" << std::endl;
		exit(1);
	}
	struct sockaddr_in my_addr;
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(port);
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bzero(&(my_addr.sin_zero), 8);
	int ret = bind(this->sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr));
	if (ret < 0) {
		std::cout << "error binding socket" << std::endl;
		exit(1);
	}

	// init epoll related components
	this->events.resize(max_buffer);
	this->ep_instance = epoll_create(1);
	if (0 != this->AddEpollEvent(sockfd, EPOLLIN | EPOLLOUT | EPOLLET)) {
		std::cout << "error initing epoll\n";
		exit(1);
	}
	ret = listen(this->sockfd, 10);
	if (ret < 0) {
		std::cout << "failed inited!" << std::endl;
		exit(1);
	}

}

void HttpServer::Run() {
	struct sockaddr_in their_addr;
	int sin_size = sizeof (struct sockaddr_in);
	while(1) {
		int nfds = epoll_wait(ep_instance, events.data(), events.size(), -1);
		for (int i = 0; i < nfds; i++) {
			if (events[i].data.fd == sockfd) {
				// receives new connections
				int theirfd =accept(sockfd, (struct sockaddr *)&their_addr, (socklen_t *)&sin_size);
				if (theirfd < 0) {
					std::cout << "error in accepting connection" << std::endl;
					return;
				}
				setNonBlocking(theirfd);
				AddEpollEvent(theirfd, EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP | EPOLLHUP);
			} else if (events[i].events & EPOLLIN){
				thread_pool.AppendTask(events[i].data.fd);
				// close(events[i].data.fd);
			}
		}

	}
}

void HttpServer::RegisterHandler(RequestType r_type, std::string path, HttpRequestHandler_t handle_func) {
	this->thread_pool.RegisterHandler(r_type, path, handle_func);
}

void HttpServer::Close() {
	close(this->sockfd);
}
}