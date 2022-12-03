#pragma once
#include "base_bot.hpp"

class GodBot: public Base_Bot{
protected:
    static uint64_t count_;
public:
    GodBot();
    void Run() override;
    const uint64_t GetID() const override;
    const chrono::milliseconds GetLifeTime() const override;
    void Pause() override;
    void Resume() override;
    void Stop() override;
    static const uint64_t Count();
    virtual ~GodBot();
};