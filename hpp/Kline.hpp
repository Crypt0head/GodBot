#pragma once

#include <iostream>
#include <boost/property_tree/ptree.hpp>

using ptree_t = boost::property_tree::ptree;

ptree_t string_to_ptree(const std::string&);

class Kline{
private:
	ulong open_time_;
	double open_price_;
	double high_price_;
	double low_price_;
	double close_price_;
	double volume_;
	double quote_volume_;
	ulong trades_num_;
	double taker_base_volume_;
	double taker_quote_volume_;

public:
	Kline() = delete;
	Kline(const ulong&,const double&,const double&,const double&,const double&,const double&,const double&,const ulong&, const double&, const double&);
	Kline(const std::string&);
	Kline(const ptree_t&);
	const Kline* operator=(const Kline&);
	const bool operator==(const Kline&);
	int parse_str(const std::string&);
	const double get_close_price() const;
	const double get_opentime() const;
	const double get_min_price() const;
	~Kline();
};