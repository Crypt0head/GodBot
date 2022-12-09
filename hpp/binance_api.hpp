#pragma once

#include "base_api.hpp"

#define BINANCE_API_CONFIG_FILE "cfg/binance_api.json"

class binance_api : public base_api {
public:
    binance_api();

	binance_api(const std::string& key, const std::string& secret, const std::string& api_file);

	void set_keys(std::pair<std::string, std::string> keys) override;

	void set_cfg(const std::string& file) override;

	json_data call(const std::string& method, const std::string& p, const http::REQTYPE& rtype, const SECURITY_TYPE& stype) override;

	json_data open_spot_order(const std::string& symbol,const ORDER_SIDE& side, const ORDER_TYPE& type, const double& quantity, const double& price, const double& stopprice, const TIME_IN_FORCE& tif) override;

	json_data query_spot_order(const std::string& symbol, const ulong& orderId) override;

	json_data open_stoploss_spot_order(const std::string& symbol,const ORDER_SIDE& side, const double& quantity, const double& price, const double& stopprice,const TIME_IN_FORCE& tif) override;

	json_data close_spot_order(const std::string& symbol, const ulong& orderId) override;

	json_data close_all_spot_orders(const std::string& symbol) override;

	json_data get_symbol_price(const std::string& symbol) override;

	json_data get_server_time() override;

	json_data get_kline(const std::string& symbol,const INTERVAL& i, const ulong& starttime, const ulong& endtime, const int32_t& limit) override;

	json_data open_oco_spot_order(const std::string& symbol,const ORDER_SIDE& side, const double& quantity, const double& price, const double& stop_price, const double limit_price, const TIME_IN_FORCE& tif) override;

	json_data close_oco_spot_order(const std::string& symbol, const ulong& orderListId) override;

	std::string build(std::vector<std::string> params_) override;

	std::unique_ptr<base_api> clone() const override;

    ~binance_api();

protected:
	std::string signature(const std::string& params) override;
};