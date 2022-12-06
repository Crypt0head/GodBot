#pragma once
#include <iostream>
#include "base_api.hpp"

class base_data{
public:
    base_data() = default;
    virtual void write_log(std::ostream&, const ORDER_SIDE&) const = 0;
    virtual std::unique_ptr<base_data> clone() const = 0;
    virtual void set(const double&,const double&, const double&, const double&, const double&, const double&, const double&) = 0;
    virtual ~base_data(){};
};