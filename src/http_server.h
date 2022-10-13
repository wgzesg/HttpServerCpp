// My http server class
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> 
#include <string.h>
#include <iostream>
#include <functional>
#include "http_parser.h"

using HttpRequestHandler_t = std::function<MyHttp::HttpResponse(const MyHttp::HttpRequest&)>;
namespace MyHttp {

class HttpServer {

private:
	int sockfd;
	std::unordered_map<std::string, std::unordered_map<RequestType, HttpRequestHandler_t>> handler_map;
	HttpRequestHandler_t get_handler_func(const HttpRequest& request);
	void child_handler(int newfd);
	int send_response(const HttpResponse& resp);

public:

	HttpServer();
	~HttpServer();

	void Init(int port, std::string ip_address);
	void Listen();
	void Handle_request();
	void Close();

	void RegisterHandler(RequestType r_type, std::string path, HttpRequestHandler_t handle_func);
};
}