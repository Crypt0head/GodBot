#pragma once

class base_strategy{
public:
    base_strategy() = default;
    virtual void Run() = 0;
    virtual ~base_strategy(){};
};