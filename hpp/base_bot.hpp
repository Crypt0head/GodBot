#pragma once

#include <iostream>
#include <thread>
#include <chrono>

namespace chrono = std::chrono;

// class Base_API;
// class Base_Logger;
// class Base_Stratagy;

class Base_Bot{
protected:
    uint64_t id_;
    chrono::milliseconds start_time_;
    bool is_pause = false;
    bool is_finish = false;
    // Base_API* api_;
    // Base_Logger* logger_;
    // Base_Stratagy* stratagy_;
public:
    Base_Bot(){}
    virtual void Run() = 0;
    virtual const uint64_t GetID() const = 0;
    virtual const chrono::milliseconds GetLifeTime() const = 0;
    virtual void Pause() = 0;
    virtual void Resume() = 0;
    virtual void Stop() = 0;
};