#include "http_ReqRes.h"
namespace MyHttp {

std::string ContentType2String[7] = {
	"text/html", "text/plain", "text/css", "text/javascript",
	"text/png", "text/jpeg", "text/gif"};


bool hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

ContentType get_contentType_from_path(std::string s) {
	if (hasEnding(s, ".js")) {
		return ContentType::TEXT_JS;
	} else 	if (hasEnding(s, ".html")) {
		return ContentType::TEXT_HTML;
	} else 	if (hasEnding(s, ".css")) {
		return ContentType::TEXT_CSS;
	} else 	if (hasEnding(s, ".png")) {
		return ContentType::IMAGE_PNG;
	} else 	if (hasEnding(s, ".jpeg")) {
		return ContentType::IMAGE_JPEG;
	} else 	if (hasEnding(s, ".gif")) {
		return ContentType::IMAGE_PNG;
	}

	return ContentType::TEXT_PLAIN;

}

std::string contenttype2string(ContentType s) {
	return ContentType2String[static_cast<int>(s)];
}

void HttpProtocol::set_header(std::string key, std::string val) {
	this->headers[key] = val;
}

void HttpResponse::set_content(std::string s) {
	this->content = s;
}

std::string HttpResponse::to_string() {
	std::ostringstream oss;
	oss << this->version << ' ';
	oss << static_cast<int>(this->status_code) << ' ';
	oss << this->status_code << "\r\n";
	for (const auto& p : this->headers)
		oss << p.first << ": " << p.second << "\r\n";
	oss << "\r\n";
	oss << this->content << "\r\n";
	std::string res = oss.str();
	return res;
}

std::string HttpRequest::to_string() {
	return "hi";
}





}