#pragma once

#include <iostream>
#include <ostream>
#include <string>
#include <map>
#include <list>
#include <stdexcept>

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/asio/ssl.hpp>

namespace asio = boost::asio;
namespace ip = boost::asio::ip;
namespace sys = boost::system;
namespace ssl = boost::asio::ssl;
namespace beast = boost::beast;

using asio::ip::tcp;
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

			request_stream << "Connection: close\r\n\r\n";
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

			request_stream << "Connection: close\r\n\r\n";
		}
	};

	class connection {
	private:
		std::string host_;
		std::string port_;
		std::string recv_data_;
		std::string recv_header_;

	public:
		connection() : host_(""), port_("") { };
		connection(const std::string host, const std::string port) : host_(host), port_(port) {}
		~connection() {}

		void request(const std::string endpoint, const request& r, const std::string& params = "", const headers_t& headers = headers_t(), const REQTYPE rtype = REQTYPE::GET) {	
			asio::io_service svc;

			ssl::context ctx(ssl::context::sslv23_client);
			ssl::stream<ip::tcp::socket> ssocket = { svc, ctx };
			ip::tcp::resolver resolver(svc);
			auto it = resolver.resolve(host_, port_);

			connect(ssocket.lowest_layer(), it);

			ssocket.handshake(ssl::stream_base::handshake_type::client);
			
			beast::http::request<beast::http::string_body> req;

			std::string endpoint_with_query = params.size() != 0 ? (endpoint + "?" + params) : endpoint;

			if (rtype == REQTYPE::GET) {
				req = beast::http::request<beast::http::string_body> { beast::http::verb::get, endpoint_with_query, 11 };
			}

			if (rtype == REQTYPE::POST) {
				req = beast::http::request<beast::http::string_body> { beast::http::verb::post, endpoint_with_query, 11 };
			}

			std::for_each(headers.begin(), headers.end(), [&] (std::pair<std::string, std::string> pair) {
				req.set(pair.first, pair.second); });

			req.set(beast::http::field::host, host_);
			req.set(beast::http::field::connection, "close");

			try {
				beast::http::write(ssocket, req);
			} 
			catch(std::exception& e) {
			 	std::cout<<e.what()<<std::endl;
			}

			beast::http::response<beast::http::string_body> res;
			beast::flat_buffer buffer;

			try {
				beast::http::read(ssocket, buffer, res);
			}
			catch(std::exception& e) {
				std::cout<<e.what()<<std::endl;
			}

			auto status_code = res.result_int();

			if (status_code != 200) {
				std::cout << "Status Code: " << status_code << std::endl;
				std::cout << "Headers:\n" << res.base() << std::endl;
				std::cout << "Body:\n" << res.body() << std::endl;

				std::string msg{ "!> Failed with error\n"};
				throw std::runtime_error(msg);
			}

			recv_header_ = "";

			recv_data_ = res.body();
			////////////////////////////////////////////////////////////////////////////////


			// // recv_data_.clear();
			// // recv_header_.clear();
			// std::string final_url = url; // + "?";
			// std::string final_params = params;

			// asio::io_service ios;
			// ip::tcp::socket socket(ios);

			// asio::streambuf request;

			// if(rtype == http::REQTYPE::GET){
			// 	final_url +=params;
			// }

			// // tcp::resolver resolver(ios);
			// // tcp::resolver::query query(final_url);
			// // std::cerr << query.host_name() << std::endl;
			// // tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
			// tcp::endpoint connectionEndpoint(asio::ip::make_address("api.binance.com"), 443);

			// asio::connect(socket, connectionEndpoint);


			// // curl_easy_setopt(curl_object::get_instance(), CURLOPT_URL, final_url.c_str());
			// // curl_easy_setopt(curl_object::get_instance(), CURLOPT_WRITEDATA, &recv_data_);
			// // curl_easy_setopt(curl_object::get_instance(), CURLOPT_HEADERDATA, &recv_header_);
			// // curl_easy_setopt(curl_object::get_instance(), CURLOPT_WRITEFUNCTION, write_received_data_to_string);

			// if(rtype == http::REQTYPE::DELETE){
			// 	// curl_easy_setopt(curl_object::get_instance(), CURLOPT_CUSTOMREQUEST, "DELETE");
			// }

			// if(rtype == REQTYPE::POST || rtype == REQTYPE::DELETE){
			// 	// curl_easy_setopt(curl_object::get_instance(), CURLOPT_POSTFIELDS, final_params.c_str());
			// 	// curl_easy_setopt(curl_object::get_instance(), CURLOPT_POSTFIELDSIZE, final_params.size());
			// }

			// std::ostream request_stream(&request);

			// // if(headers.size() > 0) {
			// 	r.prepare(headers, request_stream);
			// // }

			// asio::io_context::count_type rescode;

			// try{
			// 	rescode = asio::write(socket, request);
			// }
			// catch(std::exception& e){
			// 	std::cout<<e.what()<<std::endl;
			// }

			// if (rescode) {
			// 	std::string msg{ "!> Failed with error : \nHeaders: " + std::to_string(headers.size()) + "\nRequest: " + std::string((std::istreambuf_iterator<char>(&request)), std::istreambuf_iterator<char>()) };
			// 	throw std::runtime_error(msg);
			// }

			// sys::error_code ec;
			// asio::streambuf recv_databuf_;

			// try {
			// 	rescode = asio::read(socket, recv_databuf_, ec);
			// }
			// catch(std::exception &e) {
			// 	std::cout << e.what() << std::endl;
			// }

			// if (rescode) {
			// 	std::string msg{ "!> Failed with error : \nHeaders: " + std::to_string(headers.size()) + "\nResponse: " + std::string((std::istreambuf_iterator<char>(&recv_databuf_)), std::istreambuf_iterator<char>()) };
			// 	throw std::runtime_error(msg);
			// }
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