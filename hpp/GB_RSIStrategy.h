#pragma once

#include "base_strategy.hpp"

class GB_RSIStrategy : public base_strategy{
public:
    GB_RSIStrategy() = default;
    virtual void Run() override;
    ~GB_RSIStrategy() = default;
};