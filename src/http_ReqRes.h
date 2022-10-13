#include <unordered_map>
#include <string>
#include <sstream>
#include <vector>

namespace MyHttp {

enum class RequestType {

	GET,
	POST,
	PUT,
	DELETE

};

enum class ContentType {
	TEXT_HTML = 0,
	TEXT_PLAIN,
	TEXT_CSS,
	TEXT_JS,

	IMAGE_PNG,
	IMAGE_JPEG,
	IMAGE_GIF,

};


// ContentType string2contenttype(std::string s);
ContentType get_contentType_from_path(std::string s);
std::string contenttype2string(ContentType s);

class HttpProtocol {
protected:
	virtual ~HttpProtocol() = default;;
	std::unordered_map<std::string, std::string> headers;
	std::string version;

public:
	HttpProtocol(): version("HTTP/1.1"){};
	void set_header(std::string key, std::string val);
	void clear_header() { this->headers.clear(); };
	virtual std::string to_string() {return "xx";};
};

class HttpResponse : public HttpProtocol{
private:
	std::string content;
	int status_code;

public:
	void set_content(std::string s);
	void set_status_code(int x) { this->status_code = x; };
	void clear_header() { this->headers.clear(); };
	const std::string get_content() { return this->content; };

	virtual std::string to_string();
};

class HttpRequest : public HttpProtocol {
private:
	RequestType r_type;
	std::string path;
	std::vector<ContentType> content_type;

public:
	void set_info(std::string key, std::string val);
	void set_path(std::string s) { this->path = s; }
	void set_r_type(RequestType t) { this->r_type = t; }
	RequestType get_request_type() const { return this->r_type; }
	std::string get_request_path() const { return this->path; }
	virtual std::string to_string();

};

} // namespace MyHttp