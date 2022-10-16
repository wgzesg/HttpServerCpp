// http parser
#include "http_parser.h"

#include <iostream>

namespace MyHttp {
RequestType string2r_type(std::string s) {
	if (s == "GET") {
		return RequestType::GET;
	} else if (s == "PUT") {
		return RequestType::PUT;
	} else if (s == "POST") {
		return RequestType::POST;
	} else if (s == "DELETE") {
		return RequestType::DELETE;
	}
	return RequestType::GET;
}

HttpRequest parse(std::string s) {
	std::unordered_map<std::string, std::string> res;
	std::string delim = "\n";
	size_t next_pos;
	int line_x = 0;
	HttpRequest request;
	while((next_pos = s.find(delim)) != std::string::npos) {
		std::string token = s.substr(0, next_pos); 
		if (line_x == 0) {
			size_t first_space = token.find(" ");
			std::string req_type = token.substr(0, first_space);
			res["request type"] = req_type;
			std::cout << "request type" << ": " << res["request type"] << std::endl;
			token.erase(0, first_space + 1);
			std::string path = token.substr(0, token.find(" "));
			res["path"] = path;
			std::cout << "path" << ": " << res["path"] << std::endl;
			request.set_r_type(string2r_type(req_type));
			request.set_path(path);
		} else {
			size_t first_space = token.find(": ");
			if (first_space == std::string::npos) {
				break;
			}
			std::string key = token.substr(0, first_space);
			token.erase(0, first_space + 2);
			std::string val = token.substr(0, token.find(" "));
			request.set_header(key, val);
			std::cout << key << ": " << res[key] << std::endl;
		}
		line_x++;
		s.erase(0, next_pos + 1);
	}
	return request;
}

} // MyHttp