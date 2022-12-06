#pragma once

#include <iostream>
#include <fstream>
#include <memory>
#include <vector>

#include "base_data.hpp"

class base_logger{
protected:
    std::ostream* os_;
    std::unique_ptr<base_data> data_;
    std::string log_folder_;
public:
    virtual void set_log_stream(std::ostream&) = 0;
    virtual std::ostream& get_log_stream() = 0;
    virtual void set_log_folder(const std::string&) = 0;
    virtual const std::string& get_log_folder() const = 0;
    virtual void set_log_data(const double&, const double&, const double&, const double&, const double&, const double&, const double&) = 0;
    virtual void output(const ORDER_SIDE&) const = 0;
    virtual void set_log_file(const std::string&) = 0;

    virtual ~base_logger(){};
};