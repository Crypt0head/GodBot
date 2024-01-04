#include "../hpp/GodBot.hpp"
#include "../hpp/binance_api.hpp"
#include "../hpp/GB_LogData.hpp"
#include "../hpp/GB_Logger.hpp"
#include "../hpp/GB_SimpleStrategy.hpp"
#include "../hpp/GB_RSIStrategy.h"

#define DEFAULT_CONFIG_FILE "cfg/config.json" 
#define DEFAULT_SECRETS_FILE "cfg/secrets.json" 
#define DEFAULT_API_FILE "cfg/binance_api.json" 

#define DEFAULT_LOGS_FOLDER "logs"

#define sec 1000
#define min sec*60
#define hour min*60
#define day hour*24
#define week day*7
#define month (ulong)week*4

std::vector<std::string> tags = {"alice", "bob", "robert", "sam", "jeth", "roger", "albert", "hans", "jim", "warg", "orm", "aki", "egel", "skalagrim", "vica", "nica", "cris"};

std::string GenerateTag(){
    srand(::time(nullptr));

    std::string postfix = std::to_string(rand()%1000);

    return tags[rand()%tags.size()] + '_' + postfix;
}

uint64_t GodBot::count_ = 0;

GodBot::GodBot(const std::string& tag = ""){
    start_time_ = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock().now().time_since_epoch());
    id_ = count_++;
    SetTag(tag);
    logger_ = std::shared_ptr<GB_Logger>(new GB_Logger(new GB_LogData));
    strategy_ = std::shared_ptr<GB_RSIStrategy>(new GB_RSIStrategy());
}

static const std::map<INTERVAL, ulong> time_map = 
            std::map<INTERVAL, ulong>({{INTERVAL::s1, sec},{INTERVAL::m1, min},{INTERVAL::m3, min*3},{INTERVAL::m5, min*5},{INTERVAL::m15, min*15},{INTERVAL::m30, min*30},
                                       {INTERVAL::h1, hour},{INTERVAL::h2, hour*2},{INTERVAL::h4, hour*4},{INTERVAL::h6, hour*6},{INTERVAL::h8, hour*8},{INTERVAL::h12, hour*12},
                                        {INTERVAL::d1, day},{INTERVAL::d3, day*3},{INTERVAL::w1, week},{INTERVAL::M1, month}});

void GodBot::Run(){
    std::string api_config_file, pub_key, sec_key;
    try{
        ptree_t tmp;
        auto append = [](ptree_t* src, ptree_t* dest){for(auto i : *src){dest->push_back(i);}};
        auto key_by_value = [](const std::string& value){for(auto i : base_api::time_intervals_){if(value == i.second) return i.first; return INTERVAL::m1;}};

        api_config_file = config_.get<std::string>("api_config_file");
        pub_key = config_.get<std::string>("public_key");
        sec_key = config_.get<std::string>("secret_key");
        api_->set_keys(std::make_pair(pub_key, sec_key));
        api_->set_cfg(api_config_file);

        std::string logs_folder_ = config_.count("logs_folder") ? config_.get<std::string>("logs_folder") : DEFAULT_LOGS_FOLDER;
        logger_->set_log_folder(logs_folder_);
        logger_->set_log_file(std::to_string(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock().now().time_since_epoch()).count())+"_log.json");

        strategy_->api_ = api_;
        strategy_->logger_ = logger_;
        strategy_->timerframe_ = key_by_value(config_.get<std::string>("timeframe"));
        strategy_->symbol_ = config_.get<std::string>("symbol");
        strategy_->take_profit_ = 1 + config_.get<double>("take_profit_percentage")/100;
        strategy_->stop_loss_ = 1 - config_.get<double>("stop_loss_percentage")/100;
        strategy_->log_time_ = config_.get<ulong>("log_time");
        strategy_->is_finished_ = std::make_shared<bool>(is_finish);
        // is_stdlog = config_.count("log_to_std") ? config_.get<bool>("log_to_std") : false;  //TODO: should be depricated

    }
    catch(std::exception& err){
        std::cerr<<err.what()<<'\n';
    }

    std::cout<<"> Bot "<<GetTag()<<" started traiding on "<<strategy_->symbol_<<"\n";
    strategy_->Run();
    std::cout<<"> Bot "<<GetTag()<<" finished traiding on "<<strategy_->symbol_<<std::endl;
}

const uint64_t GodBot::GetID() const {
    return id_;
}

const chrono::milliseconds GodBot::GetLifeTime() const {
    auto cur_time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock().now().time_since_epoch());
    return cur_time-start_time_;
}

void GodBot::Pause(){
    is_pause = true;
}

void GodBot::Resume(){
    is_pause = false;
}

void GodBot::Stop(){
    is_finish = true;
}

const uint64_t GodBot::Count(){
    return count_;
}

void GodBot::SetConfig(ptree_t& cfg){
    config_ = cfg;
}

const ptree_t& GodBot::GetConfig() const{
    return config_;
};

void GodBot::UploadConfig(const ptree_t& config){
    config_ = config;

    try{
        ptree_t tmp;
        auto append = [](ptree_t* src, ptree_t* dest){for(auto i : *src){dest->push_back(i);}};

        json_parser::read_json(config_.get<std::string>("config_file") ,tmp);
        append(&tmp, &config_);
        json_parser::read_json(config_.get<std::string>("secrets_file") ,tmp);
        append(&tmp, &config_);
    }
    catch(std::exception& e)
    {
        std::cerr<<e.what()<<std::endl;
    }
}

void GodBot::SetTag(const std::string& tag = ""){
    
    if(tag.size() == 0){
        tag_ = GenerateTag();
        return;
    }

    tag_ = tag;
}

const std::string& GodBot::GetTag() const{
    return tag_;
}

void GodBot::SetAPI(const base_api* api){
    api_ = api->clone();
}

void GodBot::SwitchLog(){
    is_stdlog = !is_stdlog;
}

GodBot::~GodBot(){
    count_--;
}