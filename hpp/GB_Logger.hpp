#pragma once
#include "base_logger.hpp"

class GB_Logger : public base_logger{
public:
    GB_Logger();
    GB_Logger(const base_data*);
    void set_log_stream(std::ostream&) override;
    void set_log_file(const std::string&) override;
    std::ostream& get_log_stream() override;
    void set_log_folder(const std::string&) override;
    const std::string& get_log_folder() const override;
    void output(const ORDER_SIDE& side) const override;
    void set_log_data(const double&, const double&, const double&, const double&, const double&, const double&, const double&) override;
    ~GB_Logger();
};