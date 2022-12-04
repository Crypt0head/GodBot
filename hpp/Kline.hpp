#pragma once

#include <iostream>

#include "utils.hpp"

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
	Kline(const ulong& ot,const double& op,const double& hp,const double& lp,const double& cp,const double& v,
			const double& qv,const ulong& tn, const double& tbv, const double& tqv) : open_time_(ot),
																					open_price_(op),
																					high_price_(hp),
																					low_price_(lp),
																					close_price_(cp),
																					volume_(v),
																					quote_volume_(qv),
																					trades_num_(tn),
																					taker_base_volume_(tbv), 
																					taker_quote_volume_(tqv){
	}

	Kline(const std::string& str)
	{
		parse_str(str);
	}

	Kline(const ptree_t& pt){
		try{
			auto&& it = pt.begin();

			open_time_ 			= (*it++).second.get<ulong>("");
			open_price_ 		= (*it++).second.get<double>("");
			high_price_ 		= (*it++).second.get<double>("");
			low_price_ 			= (*it++).second.get<double>("");
			close_price_ 		= (*it++).second.get<double>("");
			volume_ 			= (*it++).second.get<double>("");
			quote_volume_ 		= (*it++).second.get<double>("");
			// trades_num_ 		= (*it++).second.get<ulong>("");
			// taker_base_volume_ 	= (*it++).second.get<double>("");
			// taker_quote_volume_ = (*it++).second.get<double>("");

		}catch(const std::exception& e){
			std::cerr<<e.what()<<std::endl;
		}
	}

	const Kline* operator=(const Kline& val){
		open_time_ 			= val.open_time_;
		open_price_ 		= val.open_price_;
		high_price_ 		= val.high_price_;
		low_price_ 			= val.low_price_;
		close_price_ 		= val.close_price_;
		volume_ 			= val.volume_;
		quote_volume_ 		= val.quote_volume_;
		trades_num_ 		= val.trades_num_;
		taker_base_volume_ 	= val.taker_base_volume_;
		taker_quote_volume_ = val.taker_quote_volume_;

		return this;
	}

	const bool operator==(const Kline& val){
		if(
			open_time_ 			== val.open_time_ &&
			open_price_ 		== val.open_price_ &&
			high_price_ 		== val.high_price_ &&
			low_price_ 			== val.low_price_ &&
			close_price_ 		== val.close_price_ &&
			volume_ 			== val.volume_ &&
			quote_volume_ 		== val.quote_volume_ &&
			trades_num_ 		== val.trades_num_ &&
			taker_base_volume_ 	== val.taker_base_volume_ &&
			taker_quote_volume_ == val.taker_quote_volume_
		) 
			return true;

		return false;
	}

	int parse_str(const std::string& str){
		ptree_t pt = string_to_ptree(str);

		try{
			auto n = pt.get_child(".");
			auto&& it = n.begin();

			open_time_ 			= (*it++).second.get<ulong>("");
			open_price_ 		= (*it++).second.get<double>("");
			high_price_ 		= (*it++).second.get<double>("");
			low_price_ 			= (*it++).second.get<double>("");
			close_price_ 		= (*it++).second.get<double>("");
			volume_ 			= (*it++).second.get<double>("");
			quote_volume_ 		= (*it++).second.get<double>("");
			// trades_num_ 		= (*it++).second.get<ulong>("");
			// taker_base_volume_ 	= (*it++).second.get<double>("");
			// taker_quote_volume_ = (*it++).second.get<double>("");

		}catch(const std::exception& e){
			std::cerr<<e.what()<<std::endl;
		}

		return 0;
	}

	const double& get_close_price() const{
		return close_price_;
	}

	const double& get_opentime() const{
		return open_time_;
	}

	const double& get_min_price() const{
		return open_time_ - close_price_ > 0 ? close_price_ : open_price_;
	}

	~Kline(){};
};