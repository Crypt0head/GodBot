#include "../hpp/binance_api.hpp"

binance_api::binance_api(){}

binance_api::binance_api(const std::string& key, const std::string& secret, const std::string& api_file = BINANCE_API_CONFIG_FILE){
	key_ = key;
	secret_ = secret;

	set_cfg(api_file);
}	

void binance_api::set_cfg(const std::string& file = BINANCE_API_CONFIG_FILE){
	try{
		boost::property_tree::read_json(file, api_cfg_);
		url_ = api_cfg_.get<std::string>("api_Base_url") + api_cfg_.get<std::string>("api_Version");
	}
	catch(std::exception &e){
		std::cerr<<e.what()<<std::endl;
	}
	connection_ = http::connection();
	timestamp_ = get_timestamp();
}

void binance_api::set_keys(std::pair<std::string, std::string> keys){
	key_ = keys.first;
	secret_ = keys.second;
}

json_data binance_api::call(const std::string& method, const std::string& p, const http::REQTYPE& rtype = http::REQTYPE::GET, const SECURITY_TYPE& stype = SECURITY_TYPE::NONE) {
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
json_data binance_api::open_spot_order(const std::string& symbol,const ORDER_SIDE& side, const ORDER_TYPE& type, const double& quantity, const double& price, const double& stopprice = 0., const TIME_IN_FORCE& tif = TIME_IN_FORCE::GTC){
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

json_data binance_api::open_stoploss_spot_order(const std::string& symbol,const ORDER_SIDE& side, const double& quantity, const double& price, const double& stopprice,const TIME_IN_FORCE& tif = TIME_IN_FORCE::GTC){
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
json_data binance_api::close_spot_order(const std::string& symbol, const ulong& orderId){
	std::string endpoint = "/order";
	std::string params = "symbol=" + symbol  + "&orderId=" + std::to_string(orderId);
	return call(endpoint,params,http::REQTYPE::DELETE, SECURITY_TYPE::SIGNED);
}

/**
	@brief Query order on given symbol by orderId
	@param symbol	- currancy symbol
	@param orderId	- orderId
*/
json_data binance_api::query_spot_order(const std::string& symbol, const ulong& orderId){
	std::string endpoint = "/order";
	std::string params = "symbol=" + symbol + "&orderId=" + std::to_string(orderId);
	
	return call(endpoint,params,http::REQTYPE::POST, SECURITY_TYPE::SIGNED);
}

/**
	@brief Cancel all orders on given symbol
	@param symbol	- currancy symbol
*/
json_data binance_api::close_all_spot_orders(const std::string& symbol){
	std::string endpoint = "/openOrders";
	std::string params = "symbol=" + symbol;
	return call(endpoint,params,http::REQTYPE::DELETE, SECURITY_TYPE::SIGNED);
}

json_data binance_api::get_symbol_price(const std::string& symbol){
	std::string endpoint = "/ticker/price";
	std::string params = "symbol=" + symbol;
	return call(endpoint,params,http::REQTYPE::GET);
}

json_data binance_api::get_server_time(){
	std::string endpoint = "/time";
	std::string params = "";
	return call(endpoint,params,http::REQTYPE::GET);
}

json_data binance_api::get_kline(const std::string& symbol,const INTERVAL& i, const ulong& starttime= 0, const ulong& endtime= 0, const int32_t& limit = 1){
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
json_data binance_api::open_oco_spot_order(const std::string& symbol,const ORDER_SIDE& side, const double& quantity, const double& price, const double& stop_price, const double limit_price, const TIME_IN_FORCE& tif = TIME_IN_FORCE::GTC){
	std::string endpoint = "/order/oco";
	std::string params = "symbol=" + symbol + "&side=" + order_side_.at(side) 
						+ "&quantity=" + std::to_string(quantity) 
						+ "&price=" + std::to_string(price) + "&stopPrice=" + std::to_string(stop_price) 
						+ "&stopLimitPrice=" + std::to_string(limit_price) + "&stopLimitTimeInForce="  + time_in_force_.at(tif);
	return call(endpoint,params,http::REQTYPE::POST, SECURITY_TYPE::SIGNED);
}

json_data binance_api::close_oco_spot_order(const std::string& symbol, const ulong& orderListId)
{
	std::string endpoint = "/orderList";
	std::string params = "symbol=" + symbol + "&orderListId=" + std::to_string(orderListId);
	return call(endpoint,params,http::REQTYPE::DELETE, SECURITY_TYPE::SIGNED);
}

std::string binance_api::build(std::vector<std::string> params_) {

	std::string params = "";
	for (auto i : params_) {
		params += "&" + i;
	}
	return params;
}

std::string binance_api::signature(const std::string& params) {
	HMAC_SHA256 hmac_sha256(secret_, params);
	return hmac_sha256.hex_digest();
}

std::unique_ptr<base_api> binance_api::clone() const{
	return std::make_unique<binance_api>(*this);
}

binance_api::~binance_api(){}