#pragma once

#include <chrono>
#include <cassert>
#include <string>
#include <map>
#include <memory>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/iostreams/stream.hpp>

#include "hmac_sha256.hpp"
#include "connection.hpp"
#include "base_api.hpp"
#include "Kline.hpp"

#define BINANCE_API_CONFIG_FILE "cfg/binance_api.json"

namespace chrono = std::chrono;

class binance_api {
private:
	std::chrono::milliseconds timestamp_;
	std::string key_;
	std::string secret_;
	std::string url_;
	http::connection connection_;
    ptree_t api_cfg_;

public:
	static const std::map<ORDER_SIDE, std::string> order_side_;
	static const std::map<ORDER_TYPE, std::string> order_type_;
	static const std::map<TIME_IN_FORCE, std::string> time_in_force_;
	static const std::map<INTERVAL, std::string> time_intervals_;


public:
    binance_api() = delete;

	binance_api(const std::string& key, const std::string& secret, const std::string& api_file = BINANCE_API_CONFIG_FILE) : key_(key), secret_(secret){
        try{
            boost::property_tree::read_json(api_file, api_cfg_);
			url_ = api_cfg_.get<std::string>("api_Base_url") + api_cfg_.get<std::string>("api_Version");
        }
        catch(std::exception &e){
            std::cerr<<e.what()<<std::endl;
        }
		connection_ = http::connection();
		timestamp_ = get_timestamp();
	}	

	json_data call(const std::string& method, const std::string& p, const http::REQTYPE& rtype = http::REQTYPE::GET, const SECURITY_TYPE& stype = SECURITY_TYPE::NONE) {
		std::string params;
		std::map<std::string, std::string> headers;
		
		params.append(p);

		if(stype != SECURITY_TYPE::NONE){
			params.append("&timestamp=" + std::to_string(get_timestamp().count()));
			std::string api_key_header = api_cfg_.get<std::string>("api_Header");
			headers[api_key_header] = key_;

			
			if(stype == SECURITY_TYPE::SIGNED){
				std::string sign = this->signature(params);
				params.append("&signature=" + sign);
			}
		}

		do{
			try{
				connection_.request(url_ + method, http::post(), params, headers, rtype);
				break;
			}catch(const std::exception& e)
			{
				std::cerr<<e.what()<<std::endl;
				sleep(10);
			}
		}while(true);

		return connection_.get_response();
	}

	/**
		@brief Open order on given symbol
		@param symbol	- currancy symbol
		@param side		- buy/sell
		@param type		- order type (LIMIT, MARKET. ...)
		@param quantity	- quantity coins to buy/sell
		@param price	- order price
		@param tif		- sets order expiration rule
	*/
	json_data open_spot_order(const std::string& symbol,const ORDER_SIDE& side, const ORDER_TYPE& type, const double& quantity, const double& price, const double& stopprice = 0., const TIME_IN_FORCE& tif = TIME_IN_FORCE::GTC){
		std::string endpoint = "/order";
		std::string params = "symbol=" + symbol + "&side=" + order_side_.at(side) 
							+ "&type=" + order_type_.at(type) + "&quantity=" + std::to_string(quantity) 
							+ "&price=" + std::to_string(price) + "&timeInForce=" + time_in_force_.at(tif);
		if(type == ORDER_TYPE::STOP_LOSS || type == ORDER_TYPE::STOP_LOSS_LIMIT || 
			type == ORDER_TYPE::TAKE_PROFIT || type == ORDER_TYPE::TAKE_PROFIT_LIMIT){
			params += "&stopPrice=" + std::to_string(stopprice);
		}
		return call(endpoint,params,http::REQTYPE::POST, SECURITY_TYPE::SIGNED);
	}

	json_data open_stoploss_spot_order(const std::string& symbol,const ORDER_SIDE& side, const double& quantity, const double& price, const double& stopprice,const TIME_IN_FORCE& tif = TIME_IN_FORCE::GTC){
		std::string endpoint = "/order";
		std::string params = "symbol=" + symbol + "&side=" + order_side_.at(side) 
							+ "&type=" + order_type_.at(ORDER_TYPE::STOP_LOSS_LIMIT) + "&quantity=" + std::to_string(quantity) 
							+ "&price=" + std::to_string(price) + "&stopPrice=" + std::to_string(stopprice) + "&timeInForce=" + time_in_force_.at(tif);
		return call(endpoint,params,http::REQTYPE::POST, SECURITY_TYPE::SIGNED);
	}

	/**
		@brief Cancel order on given symbol by it's Id
		@param symbol	- currancy symbol
		@param orderId	- order's Id
	*/
	json_data close_spot_order(const std::string& symbol, const ulong& orderId){
		std::string endpoint = "/order";
		std::string params = "symbol=" + symbol  + "&orderId=" + std::to_string(orderId);
		return call(endpoint,params,http::REQTYPE::DELETE, SECURITY_TYPE::SIGNED);
	}

	/**
		@brief Cancel all orders on given symbol
		@param symbol	- currancy symbol
	*/
	json_data close_all_spot_orders(const std::string& symbol){
		std::string endpoint = "/openOrders";
		std::string params = "symbol=" + symbol;
		return call(endpoint,params,http::REQTYPE::DELETE, SECURITY_TYPE::SIGNED);
	}

	json_data get_symbol_price(const std::string& symbol){
		std::string endpoint = "/ticker/price";
		std::string params = "symbol=" + symbol;
		return call(endpoint,params,http::REQTYPE::GET);
	}

	json_data get_server_time(){
		std::string endpoint = "/time";
		std::string params = "";
		return call(endpoint,params,http::REQTYPE::GET);
	}

	json_data get_kline(const std::string& symbol,const INTERVAL& i, const ulong& starttime= 0, const ulong& endtime= 0, const int32_t& limit = 1){
		std::string endpoint = "/klines";
		std::string params = "symbol=" + symbol + "&interval=" + time_intervals_.at(INTERVAL::m1) + "&limit=" +std::to_string(limit);
		if(starttime!=0)
		{
			params += "&startTime=" + std::to_string(starttime);	
		}
		return call(endpoint,params,http::REQTYPE::GET);
	}

	/**
		@brief Open OCO order on given symbol
		@param symbol	- currancy symbol
		@param side		- buy/sell
		@param type		- order type (LIMIT, MARKET. ...)
		@param quantity	- quantity coins to buy/sell
		@param price	- order price
		@param stop_price - stop price trigger
		@param limit_price - price of limit order
	*/
	json_data open_oco_spot_order(const std::string& symbol,const ORDER_SIDE& side, const double& quantity, const double& price, const double& stop_price, const double limit_price, const TIME_IN_FORCE& tif = TIME_IN_FORCE::GTC){
		std::string endpoint = "/order/oco";
		std::string params = "symbol=" + symbol + "&side=" + order_side_.at(side) 
							+ "&quantity=" + std::to_string(quantity) 
							+ "&price=" + std::to_string(price) + "&stopPrice=" + std::to_string(stop_price) 
							+ "&stopLimitPrice=" + std::to_string(limit_price) + "&stopLimitTimeInForce="  + time_in_force_.at(tif);
		return call(endpoint,params,http::REQTYPE::POST, SECURITY_TYPE::SIGNED);
	}

	json_data close_oco_spot_order(const std::string& symbol, const ulong& orderListId)
	{
		std::string endpoint = "/orderList";
		std::string params = "symbol=" + symbol + "&orderListId=" + std::to_string(orderListId);
		return call(endpoint,params,http::REQTYPE::DELETE, SECURITY_TYPE::SIGNED);
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

	static std::chrono::milliseconds get_timestamp(){
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	}
};

const std::map<ORDER_SIDE, std::string> binance_api::order_side_ = {
	{ORDER_SIDE::BUY, "BUY"},{ORDER_SIDE::SELL, "SELL"}};
const std::map<ORDER_TYPE, std::string> binance_api::order_type_ = std::map<ORDER_TYPE, std::string>({
	{ORDER_TYPE::LIMIT, "LIMIT"},{ORDER_TYPE::MARKET, "MARKET"},{ORDER_TYPE::STOP_LOSS, "STOP_LOSS"},
 	{ORDER_TYPE::STOP_LOSS, "STOP_LOSS_LIMIT"},{ORDER_TYPE::TAKE_PROFIT, "TAKE_PROFIT"},
	{ORDER_TYPE::TAKE_PROFIT_LIMIT, "TAKE_PROFIT_LIMIT"},{ORDER_TYPE::LIMIT_MAKER, "LIMIT_MAKER"}});
const std::map<TIME_IN_FORCE, std::string> binance_api::time_in_force_ = std::map<TIME_IN_FORCE, std::string>({
	{TIME_IN_FORCE::GTC, "GTC"},{TIME_IN_FORCE::IOC, "IOC"},{TIME_IN_FORCE::FOK, "FOK"}});	
const std::map<INTERVAL, std::string> binance_api::time_intervals_ = std::map<INTERVAL, std::string>({
	{INTERVAL::s1, "1s"},{INTERVAL::m1, "1m"},{INTERVAL::m3, "3m"},{INTERVAL::m5, "5m"},{INTERVAL::m15, "15m"},{INTERVAL::m30, "30m"},
	{INTERVAL::h1, "1h"},{INTERVAL::h2, "2h"},{INTERVAL::h4, "4h"},{INTERVAL::h6, "6h"},{INTERVAL::h8, "8h"},{INTERVAL::h12, "12h"},
	{INTERVAL::d1, "1d"},{INTERVAL::d3, "3d"},{INTERVAL::w1, "1w"},{INTERVAL::M1, "1M"}});