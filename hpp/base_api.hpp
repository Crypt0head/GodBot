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
#include "Kline.hpp"

namespace chrono = std::chrono;
namespace json_parser = boost::property_tree::json_parser;

using ptree_t = boost::property_tree::ptree;

enum class ORDER_SIDE{
	BUY,
	SELL,
	NONE
};

enum class TIME_IN_FORCE{
	GTC,
	IOC,
	FOK
};

enum class ORDER_TYPE{
	LIMIT,
	MARKET,
	STOP_LOSS,
	STOP_LOSS_LIMIT,
	TAKE_PROFIT,
	TAKE_PROFIT_LIMIT,
	LIMIT_MAKER
};

enum class SECURITY_TYPE{
	SIGNED,
	PUB,
	NONE
};

enum class INTERVAL{
	s1,
	m1,
	m3,
	m5,
	m15,
	m30,
	h1,
	h2,
	h4,
	h6,
	h8,
	h12,
	d1,
	d3,
	w1,
	M1
};

class base_api{
protected:
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
	base_api() = default;
	virtual void set_keys(std::pair<std::string, std::string>) = 0;
	virtual void set_cfg(const std::string&) = 0;
	virtual json_data call(const std::string&, const std::string&, const http::REQTYPE&, const SECURITY_TYPE&) = 0;
	virtual json_data open_spot_order(const std::string&,const ORDER_SIDE&, const ORDER_TYPE&, const double&, const double&, const double&, const TIME_IN_FORCE&) = 0;
	virtual json_data open_stoploss_spot_order(const std::string&,const ORDER_SIDE&, const double&, const double&, const double&,const TIME_IN_FORCE&) = 0;
	virtual json_data close_spot_order(const std::string&, const ulong&) = 0;
	virtual json_data close_all_spot_orders(const std::string&) = 0;
	virtual json_data get_symbol_price(const std::string&) = 0;
	virtual json_data get_server_time() = 0;
	virtual json_data get_kline(const std::string&,const INTERVAL&, const ulong&, const ulong&, const int32_t&) = 0;
	virtual json_data open_oco_spot_order(const std::string&,const ORDER_SIDE&, const double&, const double&, const double&, const double, const TIME_IN_FORCE&) = 0;
	virtual json_data close_oco_spot_order(const std::string&, const ulong&) = 0;
	virtual std::string build(std::vector<std::string>) = 0;
	virtual std::unique_ptr<base_api> clone() const = 0;
	virtual ~base_api(){};

protected:
	virtual std::string signature(const std::string&) = 0;

	static std::chrono::milliseconds get_timestamp(){
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	};
};