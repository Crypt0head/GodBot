#pragma once
#include "base_bot.hpp"

class GodBot: public Base_Bot{
protected:
    static uint64_t count_;
public:
    GodBot(const std::string& tag);
    void Run() override;
    const uint64_t GetID() const override;
    const chrono::milliseconds GetLifeTime() const override;
    void Pause() override;
    void Resume() override;
    void Stop() override;
    static const uint64_t Count();
    void SetConfig(ptree_t& cfg) override;
    const ptree_t& GetConfig() const override;
    void UploadConfig(const ptree_t& cfg) override;
    void SetTag(const std::string& tag) override;
    const std::string& GetTag() const override;
    virtual ~GodBot();
};