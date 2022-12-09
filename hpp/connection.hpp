#pragma once

#include <iostream>
#include <string>
#include <map>
#include <stdexcept>

#include "curl_object.hpp"

using json_data = std::string;
using headers_t = std::map<std::string, std::string>;

namespace http {

	enum class REQTYPE{
		POST,
		GET,
		DELETE,
		PUT
	};

	class request {
	public:
		request() {}
		virtual ~request() {}

		virtual curl_slist* prepare(const headers_t&) const = 0;
	};

	class post : public request {
	public:
		post() {}
		virtual ~post() {}

		virtual curl_slist* prepare(const headers_t& headers) const {
			curl_slist* list = nullptr;
			for (auto h : headers) {
				std::string s{ std::string(h.first + ": " + h.second) };
				list = curl_slist_append(list, s.c_str());
			}
			return list;
		}
	};

	class get : public request {
	public:
		get() {}
		virtual ~get() {}
		virtual curl_slist* prepare(const headers_t& headers) const {
			curl_slist* list = nullptr;
			for (auto h : headers) {
				std::string s{ std::string(h.first + ": " + h.second) };
				list = curl_slist_append(list, s.c_str());
			}
			return list;
		}
	};

	class connection {
	private:
		std::string recv_data_;
		std::string recv_header_;
	public:
		connection(const std::string url = "")
		{}
		~connection() {}

		void request(const std::string url, const request& r, const std::string& params = "", const headers_t& headers = headers_t(), const REQTYPE &rtype = REQTYPE::GET) {
			recv_data_.clear();
			recv_header_.clear();
			std::string final_url = url + "?";
			std::string final_params = params;

			if(rtype == http::REQTYPE::GET){
				final_url +=params;
			}

			curl_easy_setopt(curl_object::get_instance(), CURLOPT_URL, final_url.c_str());
			curl_easy_setopt(curl_object::get_instance(), CURLOPT_WRITEDATA, &recv_data_);
			curl_easy_setopt(curl_object::get_instance(), CURLOPT_HEADERDATA, &recv_header_);
			curl_easy_setopt(curl_object::get_instance(), CURLOPT_WRITEFUNCTION, write_received_data_to_string);

			if(rtype == http::REQTYPE::DELETE){
				curl_easy_setopt(curl_object::get_instance(), CURLOPT_CUSTOMREQUEST, "DELETE");
			}

			if(rtype == REQTYPE::POST || rtype == REQTYPE::DELETE){
				curl_easy_setopt(curl_object::get_instance(), CURLOPT_POSTFIELDS, final_params.c_str());
				curl_easy_setopt(curl_object::get_instance(), CURLOPT_POSTFIELDSIZE, final_params.size());
			}

			if(headers.size()>0)
			{
				curl_slist* list = r.prepare(headers);
				curl_easy_setopt(curl_object::get_instance(), CURLOPT_HTTPHEADER, list);
			}

			CURLcode rescode;

			try{
				rescode = curl_easy_perform(curl_object::get_instance());
			}
			catch(std::exception& e){
				std::cout<<e.what()<<std::endl;
			}

			if (rescode != CURLE_OK) {
				std::string msg{ "!> curl_easy_perform failed with error: " };
				msg += curl_easy_strerror(rescode);
				throw std::runtime_error(msg);
			}
			
			curl_object::reset();
		}

		json_data get_response() {
			return recv_data_;
		}

	private:
		static std::size_t write_received_data_to_string(char *ptr, std::size_t size, std::size_t nmemb, void *buffer) {
			if (buffer) {
				std::string *tstr = reinterpret_cast<std::string*>(buffer);
				tstr->append(ptr, size * nmemb);
			}
			return size * nmemb;
		}
	};

}