#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <cmath>

#include "Kline.hpp"

#define DOUBLE_PRECISION 3

/** Special function to hold floating-point types at given precision */
static double accurize(const double &value, const int &power = DOUBLE_PRECISION) 
{
    double precision = std::pow(10, power);
    return std::round(value * precision) / precision;
}

/**
	@brief Average Gain/Loss
	@param data		referance to vector of double
    @return something
*/
template<typename T, typename V = std::vector<T>, typename C = std::less<>>
V AVRGL(const V& data, C cmp = C{})
{
    V result;
    result.reserve(data.size() - 1);

    for (auto i = data.begin(); i != data.end() - 1; ++i) {
        if (cmp(*i, *std::next(i))) {
            result.push_back(std::max(*std::next(i), *i) - std::min(*std::next(i), *i));
        }
    }

    return result;
}

/**
	@brief Simple Moving Average
	@param n        period of Klines to compute 
	@param data		referance to vector of double
    @return value of SMA for given first n Klines
*/
double SMA(const int n, const std::vector<double>& data)
{
    if (data.size() > n) {
        return std::accumulate(data.end() - n, data.end(), 0.) / n;
    }

    return 0;
}

double EMA(const int& n, const double& cur_price, const double& prev_ema)
{
    const double a = 2 / (double)(n + 1);
    auto res = a * cur_price + ( 1 - a) * prev_ema;
    return accurize(res);
}

// Depricated
// double EMA(const int& n,const std::vector<Kline>& data,const double& ema){
    
//     return EMA(n, data.back().get_close_price(), ema);
// }

double EMA(const int& n, const std::vector<double>& data, const int& pos = 0)
{
    double res = 0.;

    assert(&data != nullptr);

    if (data.empty()) {
        return res;
    }

    if (pos != 0)
    {
        const double a = 2 / (double)(n + 1);
        auto ema = accurize(EMA(n, data, pos - 1));
        auto ema_res = a * data[pos] + (1 - a) * ema;
        res = accurize(ema_res);
    }
    else {
        res = data[0];
    }

    return res;
}

/**
	@brief Compute EMA value on given period recursevly 
	@param n        period of Klines to compute 
	@param data		referance to vector of Kline
    @param pos      const reference to position in array
    @return value of EMA for given first n Klines
*/
double EMA(const int& n, std::vector<Kline>& data, const int& pos = 0)
{
    std::vector<double> double_array;

    for(auto kline = data.size() - pos; kline < data.size(); ++kline) {
        double_array.push_back(data[kline].get_close_price());
    }

    return EMA(n, double_array, pos);
}

/**
	@brief Compute RSI value on given range
	@param n        period of Klines to compute 
	@param data		const referance to vector of Kline
    @return value of RSI for given first n Klines
*/
double RSI(const int& n, const std::vector<Kline>& data) 
{
    std::vector<double> gain;
    gain.reserve(data.size());

    std::vector<double> loss;
    loss.reserve(data.size());

    for(auto kline : data) {
        auto change = kline.get_close_price() - kline.get_open_price();
        
        if (change > 0) {
            gain.push_back(change);
            loss.push_back(0.);
        }
        else if (change < 0) {
            loss.push_back(-1 * change);
            gain.push_back(0.);
        }
        else {
            gain.push_back(0.);
            loss.push_back(0.);
        }
    }

    //auto gain = AVRGL<double>(close_pricies, std::greater<>{});
    //auto loss = AVRGL<double>(close_pricies, std::less<>{});

    double ema_up = SMA(n, gain);
    double ema_down = SMA(n, loss);

    // double ema_up = EMA(n, gain, gain.size() - 1);
    // double ema_down = EMA(n, loss, loss.size() - 1);

    if(!ema_down) {
        return 0;
    }

    double rs = ema_up / ema_down;

    return 100 - (100 / (1 + rs));
}