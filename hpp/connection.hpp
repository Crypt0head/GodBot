#pragma once

#include <iostream>
#include <ostream>
#include <string>
#include <map>
#include <list>
#include <stdexcept>

#include <boost/asio.hpp>

// #include "curl_object.hpp"

namespace asio = boost::asio;
namespace ip = boost::asio::ip;

using json_data = std::string;
using headers_t = std::map<std::string, std::string>;
using slist = std::list<std::string>;

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

		virtual void prepare(const headers_t&, std::ostream&) const = 0;
	};

	class post : public request {
	public:
		post() {}
		virtual ~post() {}

		void prepare(const headers_t &headers, std::ostream &request_stream) const override {
			request_stream << "POST /title/ HTTP/1.1 \r\n";

			for (auto h : headers) {
				std::string s{ std::string(h.first + ": " + h.second) };
				request_stream << h.first << ": " << h.second << "\r\n";
				
			}

			request_stream << "\r\n";
		}
	};

	class get : public request {
	public:
		get() {}
		virtual ~get() {}

		void prepare(const headers_t &headers, std::ostream &request_stream) const override {
			request_stream << "GET /title/ HTTP/1.1 \r\n";

			for (auto h : headers) {
				std::string s{ std::string(h.first + ": " + h.second) };
				request_stream << h.first << ": " << h.second << "\r\n";
				
			}

			request_stream << "\r\n";
		}
	};

	class connection {
	private:
		std::string recv_data_;
		std::string recv_header_;
	public:
		connection(const std::string url = "") {}
		~connection() {}

		void request(const std::string url, const request& r, const std::string& params = "", const headers_t& headers = headers_t(), const REQTYPE &rtype = REQTYPE::GET) {
			recv_data_.clear();
			recv_header_.clear();
			std::string final_url = url + "?";
			std::string final_params = params;

			asio::io_service ios;
			ip::tcp::socket socket(ios);

			asio::streambuf request;
			std::ostream request_stream(&request);

			if(rtype == http::REQTYPE::GET){
				final_url +=params;
			}

			// curl_easy_setopt(curl_object::get_instance(), CURLOPT_URL, final_url.c_str());
			// curl_easy_setopt(curl_object::get_instance(), CURLOPT_WRITEDATA, &recv_data_);
			// curl_easy_setopt(curl_object::get_instance(), CURLOPT_HEADERDATA, &recv_header_);
			// curl_easy_setopt(curl_object::get_instance(), CURLOPT_WRITEFUNCTION, write_received_data_to_string);

			if(rtype == http::REQTYPE::DELETE){
				// curl_easy_setopt(curl_object::get_instance(), CURLOPT_CUSTOMREQUEST, "DELETE");

			}

			if(rtype == REQTYPE::POST || rtype == REQTYPE::DELETE){
				// curl_easy_setopt(curl_object::get_instance(), CURLOPT_POSTFIELDS, final_params.c_str());
				// curl_easy_setopt(curl_object::get_instance(), CURLOPT_POSTFIELDSIZE, final_params.size());
			}

			if(headers.size() > 0) {
				r.prepare(headers, request_stream);
			}

			CURLcode rescode;

			try{
				rescode = ios.run();
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