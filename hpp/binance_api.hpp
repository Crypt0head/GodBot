#pragma once

#include <ctime>
#include <cassert>
#include <string>
#include <map>
#include <boost/property_tree/json_parser.hpp>

#include "hmac_sha256.hpp"
#include "connection.hpp"

#define BINANCE_API_CONFIG_FILE "../cfg/binance_api.json"

using ptree_t = boost::property_tree::ptree;

class binance_api {
private:
	unsigned long timestamp_;
	std::string key_;
	std::string secret_;
	std::string url_;
	http::connection connection_;
    ptree_t api_cfg_;

public:
    binance_api() = delete;

	binance_api(const std::string& key, const std::string& secret) : key_(key), secret_(secret){

        try{
            boost::property_tree::read_json(BINANCE_API_CONFIG_FILE, api_cfg_);
        }
        catch(std::exception &e){
            std::cerr<<e.what()<<std::endl;
        }

		url_ = api_cfg_.get<std::string>("api_Base_url") + api_cfg_.get<std::string>("api_Version");
		connection_ = http::connection();
		timestamp_ = ::time(nullptr);
	}

	json_data call(const std::string& method, const std::string& p, const http::REQTYPE &rtype = http::REQTYPE::GET) {
		std::string params;
		
		params.append(p);
        timestamp_*=1000;
		params.append("&timestamp=" + std::to_string(timestamp_));

        std::string api_key_header = api_cfg_.get<std::string>("api_Header");

		std::map<std::string, std::string> headers;
		headers[api_key_header] = key_;
		
        std::string sign = this->signature(params);
        params.append("&signature=" + sign);

		connection_.request(url_ + method, http::post(), params, headers, rtype);
		return connection_.get_response();
	}

	std::string build(std::vector<std::string> params_) {

		std::string params = "";
		for (auto i : params_) {
			params += "&" + i;
		}
		return params;
	}

    ~binance_api(){}

private:
	std::string signature(const std::string& params) {
		HMAC_SHA256 hmac_sha256(secret_, params);
		return hmac_sha256.hex_digest();
	}
};