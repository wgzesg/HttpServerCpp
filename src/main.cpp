// #include "headsock.h"
#include "header.h"

#include "http_server.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <filesystem>

int main(void)
{
	MyHttp::HttpServer my_server;
	std::string my_addr = "127.0.0.1";
	my_server.Listen(8080, my_addr);

	HttpRequestHandler_t error_hanlder = [](const MyHttp::HttpRequest& request) -> MyHttp::HttpResponse {
		MyHttp::HttpResponse response;
	    response.set_header("Content-Type", "text/plain");
	    response.set_content("Not implemented!\n");
	    response.set_status_code(404);
	    return response;
	};

	HttpRequestHandler_t send_test = [&error_hanlder](const MyHttp::HttpRequest& request) -> MyHttp::HttpResponse {
		
		MyHttp::HttpResponse response;
	    // std::ifstream input_file;
	    std::string file_path = "test.html";
	    std::ifstream input_file(file_path);

	    MyHttp::ContentType resource_type = MyHttp::get_contentType_from_path(file_path);
	    if (!input_file.is_open()) {
	    	std::cout << "file not found!" << std::endl;
		    response = error_hanlder(request);
    		return response;
	    }
	    std::string file_content = std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
	    response.set_header("Content-Type", MyHttp::contenttype2string(resource_type));
	    response.set_content(file_content);
	    response.set_status_code(200);
	    return response;
	};

	my_server.RegisterHandler(MyHttp::RequestType::GET, "/test", send_test);
	my_server.RegisterHandler(MyHttp::RequestType::GET, "/error", error_hanlder);
	my_server.Run();
	my_server.Close();
}
