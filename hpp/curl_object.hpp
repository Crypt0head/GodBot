#ifndef CURL_OBJECT_HPP
#define CURL_OBJECT_HPP

#include <curl/curl.h>

class curl_object {
	static CURL* ch;
	curl_object() { }
public:
	~curl_object() {
		curl_easy_cleanup(ch);
		ch = nullptr;
	}

	static CURL* get_instance() {
		if (ch == nullptr) {
			ch = curl_easy_init();
		}
		return ch;
	}

	static void clean_up(){
		curl_easy_cleanup(ch);
		ch = nullptr;
	}

	static void reset(){
		curl_easy_reset(ch);
	}
};

#endif // CURL_OBJECT_HPP