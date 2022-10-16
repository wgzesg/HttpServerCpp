#include "http_server.h"
#include "utils.h"
#include <unordered_map>
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

void HttpServer::Init(int port, std::string ip_address, int max_buffer) {
	thread_pool = ThreadPool(4, max_buffer);
	this->sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->sockfd < 0) {
		std::cout << "error creating socket" << std::endl;
		exit(1);
	}
	struct sockaddr_in my_addr;
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(port);
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	// inet_pton(AF_INET, ip_address.c_str(), &(my_addr.sin_addr.s_addr));
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

}

void HttpServer::Listen() {
	int ret = listen(this->sockfd, 10);
	if (ret < 0) {
		std::cout << "failed inited!" << std::endl;
		exit(1);
	}
}

void HttpServer::Run() {
	while(1) {
		this->HandleRequest();
	}
}

void HttpServer::EpRun() {
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
				std::cout << "got connections!" << std::endl;
				setNonBlocking(theirfd);
				std::cout << "opened " << theirfd << '\n';
				AddEpollEvent(theirfd, EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP | EPOLLHUP);
			} else if (events[i].events & EPOLLIN){
				thread_pool.AppendTask(events[i].data.fd);
				// std::vector<char> buffer(30000);
				// std::fill(buffer.begin(), buffer.end(), 0);

				// std::cout << "receiving data\n";

				// read(events[i].data.fd, buffer.data(), 30000);
				// std::string s(buffer.begin(), buffer.end());
				// write(events[i].data.fd, s.c_str(), s.length());
				// close(events[i].data.fd);
				// epoll_ctl(ep_instance, EPOLL_CTL_DEL,
				// 	  events[i].data.fd, NULL);

			} else {
			// } else( events[i].events & ( EPOLLRDHUP | EPOLLHUP | EPOLLERR ) ) {
				// // std::cout << "clsoing connections\n";
				// epoll_ctl(ep_instance, EPOLL_CTL_DEL,
				// 	  events[i].data.fd, NULL);
				std::cout << "some events not handled\n";
            }
		}

	}
}

void HttpServer::HandleRequest(){
	struct sockaddr_in their_addr;
	int sin_size = sizeof (struct sockaddr_in);
	std::cout << "waiting connections!" << std::endl;
	int theirfd =accept(this->sockfd, (struct sockaddr *)&their_addr, (socklen_t *)&sin_size);
	if (theirfd < 0) {
		std::cout << "error in accepting connection" << std::endl;
		return;
	}
	std::cout << "got connections!" << std::endl;
	int id = fork();
	if (id < 0) {
		std::cout << "failed to fork out to handle request" << std::endl;
		close(theirfd);
		return;
	}

	if (id == 0) {
		this->child_handler(theirfd);
		exit(0);
	} else {
		close(theirfd);
	}
}

void HttpServer::RegisterHandler(RequestType r_type, std::string path, HttpRequestHandler_t handle_func) {
	handler_map[path][r_type] = handle_func;
}

void HttpServer::child_handler(int newfd) {
	close(this->sockfd);
	std::vector<char> buffer(30000);
	std::fill(buffer.begin(), buffer.end(), 0);

	std::cout << "receiving data\n";

	read(newfd, buffer.data(), 30000);
	std::string s(buffer.begin(), buffer.end());

	auto request = parse(s);

	auto handle_func = this->get_handler_func(request);

	auto resp = handle_func(request);

	std::string string_resp = resp.to_string();

	write(newfd, string_resp.c_str(), string_resp.length());
	close(newfd);

}

HttpRequestHandler_t HttpServer::get_handler_func(const HttpRequest& request) {
	// auto func = this->handler_map[request.get_request_path()][request.get_request_type()];
	if (this->handler_map.find(request.get_request_path()) == this->handler_map.end() 
		|| this->handler_map[request.get_request_path()].find(request.get_request_type()) == this->handler_map[request.get_request_path()].end()) {
		return this->handler_map["/error"][RequestType::GET];
	}

	return this->handler_map[request.get_request_path()][request.get_request_type()];
}

void HttpServer::Close() {
	close(this->sockfd);
}
}