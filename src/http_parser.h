#include <string>
#include <vector>
#include <unordered_map>
#include "http_ReqRes.h"
namespace MyHttp {
MyHttp::RequestType string2r_type(std::string s);
MyHttp::HttpRequest parse(std::string s);
}