#include "../hpp/base_api.hpp"

const std::map<ORDER_SIDE, std::string> base_api::order_side_ = {
	{ORDER_SIDE::BUY, "BUY"},{ORDER_SIDE::SELL, "SELL"}};
const std::map<ORDER_TYPE, std::string> base_api::order_type_ = std::map<ORDER_TYPE, std::string>({
	{ORDER_TYPE::LIMIT, "LIMIT"},{ORDER_TYPE::MARKET, "MARKET"},{ORDER_TYPE::STOP_LOSS, "STOP_LOSS"},
 	{ORDER_TYPE::STOP_LOSS, "STOP_LOSS_LIMIT"},{ORDER_TYPE::TAKE_PROFIT, "TAKE_PROFIT"},
	{ORDER_TYPE::TAKE_PROFIT_LIMIT, "TAKE_PROFIT_LIMIT"},{ORDER_TYPE::LIMIT_MAKER, "LIMIT_MAKER"}});
const std::map<TIME_IN_FORCE, std::string> base_api::time_in_force_ = std::map<TIME_IN_FORCE, std::string>({
	{TIME_IN_FORCE::GTC, "GTC"},{TIME_IN_FORCE::IOC, "IOC"},{TIME_IN_FORCE::FOK, "FOK"}});	
const std::map<INTERVAL, std::string> base_api::time_intervals_ = std::map<INTERVAL, std::string>({
	{INTERVAL::s1, "1s"},{INTERVAL::m1, "1m"},{INTERVAL::m3, "3m"},{INTERVAL::m5, "5m"},{INTERVAL::m15, "15m"},{INTERVAL::m30, "30m"},
	{INTERVAL::h1, "1h"},{INTERVAL::h2, "2h"},{INTERVAL::h4, "4h"},{INTERVAL::h6, "6h"},{INTERVAL::h8, "8h"},{INTERVAL::h12, "12h"},
	{INTERVAL::d1, "1d"},{INTERVAL::d3, "3d"},{INTERVAL::w1, "1w"},{INTERVAL::M1, "1M"}});