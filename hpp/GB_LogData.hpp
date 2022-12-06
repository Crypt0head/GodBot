#pragma once
#include <iostream>
#include "base_data.hpp"

enum class ORDER_SIDE;

struct GB_LogData : public base_data{
    double last_price = 0;
    double balance = 0;
    double coins = 0;
    double old_balance = 0;
    double ema7 = 0;
    double ema25 = 0;
    double ema99 = 0;

    void set(const double&, const double&, const double&, const double&, const double&, const double&, const double&);
    void write_log(std::ostream&, const ORDER_SIDE&) const override;
    std::unique_ptr<base_data> clone() const;
};