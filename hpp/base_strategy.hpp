#pragma once


class base_api;
class base_logger;
// class INTERVAL;

class base_strategy{
public:
    double take_profit_, stop_loss_;
    ulong log_time_;
    std::string symbol_, logs_folder_;
    INTERVAL timerframe_;
    std::shared_ptr<base_logger> logger_;
    std::shared_ptr<base_api> api_;
    std::shared_ptr<bool> is_finished_;
public:
    base_strategy() = default;
    virtual void Run() = 0;
    virtual ~base_strategy(){};
};