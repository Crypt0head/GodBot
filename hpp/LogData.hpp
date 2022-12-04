#pragma once
#include <iostream>

enum class ORDER_SIDE;

struct LogData{
    double last_price = 0;
    double balance = 0;
    double coins = 0;
    double old_balance = 0;
    double ema7 = 0;
    double ema25 = 0;
    double ema99 = 0;

    void set(const double&, const double&, const double&, const double&, const double&, const double&, const double&);
};

void write_log(std::ostream&, LogData&, ORDER_SIDE, bool);