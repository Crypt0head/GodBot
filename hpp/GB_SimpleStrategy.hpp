#pragma once

#include "base_strategy.hpp"

class GB_SimpleStrategy : public base_strategy{
public:
    GB_SimpleStrategy() = default;
    virtual void Run() override;
    ~GB_SimpleStrategy() = default;
};