#pragma once

#include <iostream>
#include <vector>
#include <cmath>

#include "Kline.hpp"

#define DOUBLE_PRECISION 3

/** Special function to hold floating-point types at given precision */
static double accurize(const double &value, const int &power = DOUBLE_PRECISION) {
    double precision = std::pow(10, power);
    return std::round(value * precision) / precision;
}

double EMA(const int& n, const double& cur_price, const double& prev_ema){
    const double a = 2 / (double)(n + 1);
    auto res = a * cur_price + ( 1 - a) * prev_ema;
    return accurize(res);
}

// Depricated
double EMA(const int& n,const std::vector<Kline>& data,const double& ema){
    
    return EMA(n, data.back().get_close_price(), ema);
    // const double a = 2 / (double)(n + 1);
    // auto res = a * data.back().get_close_price()+(1-a) * accurize(ema, 3);
    // return accurize(res, 3);
}

double EMA(const int& n, const std::vector<double>& data, const int& pos = 0){
    if(data.empty()) {
        return 0.;
    }
    
    if(pos != 0)
    {
        const double a = 2 / (double)(n + 1);
        auto ema = accurize(EMA(n, data, pos - 1));
        auto res = a * data[pos] + (1 - a) * ema;
        return accurize(res);
    }

    return data[0];
}

/**
	@brief Compute EMA value on given period recursevly 
	@param n        period of Klines to compute 
	@param data		referance to vector of Kline
    @param pos      const reference to position in array
    @return value of EMA for given first n Klines
*/
double EMA(const int& n, std::vector<Kline>& data, const int& pos = 0){
    // if(pos != 0)
    // {
    //     const double a = 2 / (double)(n + 1);
    //     auto ema = accurize(EMA(n,data,pos-1), 3);
    //     auto res = a * data[pos].get_close_price() + (1 - a) * ema;
    //     return accurize(res, 3);
    // }

    // return data[0].get_close_price();

    std::vector<double> double_array;

    for(auto kline : data) {
        double_array.push_back(kline.get_close_price());
    }

    return EMA(n, double_array, pos);
}

/**
	@brief Compute RSI value on given range
	@param n        period of Klines to compute 
	@param data		const referance to vector of Kline
    @return value of RSI for given first n Klines
*/
double RSI(const int& n, const std::vector<Kline>& data) {
    
    std::vector<double> up; // vector of prieces which closed higher then previouse
    std::vector<double> down; // vector of prieces which closed lower then previouse
    
    int i = 0;

    auto print = [](std::vector<double> &data) -> void{for(auto i : data){std::cout << i << ",";} std::cout<<"\n";};

    for(i = data.size() - n; i < data.size(); ++i) {
        if (data[i].get_close_price() > data[i-1].get_close_price()){
            up.push_back(data[i].get_close_price());
        }
        else if (data[i].get_close_price() < data[i-1].get_close_price()){
            down.push_back(data[i].get_close_price());
        }
        else {
            up.push_back(0);
            down.push_back(0);
        }
    }

    // print(up);
    // print(down);

    double ema_up = EMA(n, up, up.size());
    double ema_down = EMA(n, down, down.size());

    if(!ema_down) {
        return 0;
    }

    double rs = ema_up / ema_down;

    // std::cout << "\nEMA(up) = " << ema_up << "\n";

    // std::cout << "EMA(down) = " << ema_down << "\n";

    // std::cout << "Amount of up-closed Kline: " << up.size() << "\n";
    // std::cout << "Amount of down-closed Kline: " << down.size() << "\n";

    return 100 - (100 / (1 + rs));
}