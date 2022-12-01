#pragma once

#include <iostream>
#include <vector>

#include "binance_api.hpp"

#define TI_REAL double

double EMA(const int& n,const std::vector<Kline>& data,const double& ema){
    const double a=2/(double)(n+1);
    auto res = a*data.back().get_close_price()+(1-a)*ema;
    return res;
}

double EMA(const int& n, std::vector<Kline>& data, const int& pos = 0){
    if(pos != 0)
    {
        const double a=2/(double)(n+1);
        auto ema = EMA(n,data,pos-1);
        auto res = a*data[pos].get_close_price()+(1-a)*ema;
        return res;
    }

    return data[0].get_close_price();
}

double EMA(const int& n, const std::vector<double>& data, const int& pos = 0){
    if(pos != 0)
    {
        const double a=2/(double)(n+1);
        auto ema = EMA(n,data,pos-1);
        auto res = a*data[pos]+(1-a)*ema;
        return res;
    }

    return data[0];
}

double EMA(const int& n, const double& cur_price, const double& prev_ema){
    const double a=2/(double)(n+1);
    auto res = a*cur_price+(1-a)*prev_ema;
    return res;
}