namespace MyHttp {

enum class ContentType {
	TEXT_HTML,
	TEXT_PLAIN,
	TEXT_CSS,

	IMAGE_PNG,
	IMAGE_JPEG,
	IMAGE_GIF,
	NOT_SUPPORTED
};

class HttpResponse {
	ContentType content_type;


private:
	std::string content;


public:
	void set_content_type(std::string s);

};

} // namespace MyHttp