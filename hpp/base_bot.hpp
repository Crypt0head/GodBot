#pragma once

#include <iostream>
#include <thread>
#include <chrono>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace chrono = std::chrono;
namespace json_parser = boost::property_tree::json_parser;

using ptree_t = boost::property_tree::ptree;

class base_api;
class base_logger;
class base_strategy;

class Base_Bot{
protected:
    uint64_t id_;
    
    chrono::milliseconds start_time_;
    bool is_pause = false;
    bool is_finish = false;
    bool is_stdlog = false;
    ptree_t config_;
    std::string tag_;
    std::shared_ptr<base_api> api_;
    std::shared_ptr<base_logger> logger_;
    std::shared_ptr<base_strategy> strategy_;
public:
    Base_Bot() = default;
    virtual void Run() = 0;
    virtual const uint64_t GetID() const = 0;
    virtual const chrono::milliseconds GetLifeTime() const = 0;
    virtual void Pause() = 0;
    virtual void Resume() = 0;
    virtual void Stop() = 0;
    virtual void SetTag(const std::string& tag) = 0;
    virtual const std::string& GetTag() const = 0;
    virtual void SetConfig(ptree_t& cfg) = 0;
    virtual const ptree_t& GetConfig() const = 0;
    virtual void UploadConfig(const ptree_t& cfg) = 0;
    virtual void SetAPI(const base_api*) = 0;
    virtual void SwitchLog() = 0;
    virtual ~Base_Bot(){};
};