#include <filesystem>

#include "../hpp/GodBot.hpp"
#include "../hpp/binance_api.hpp"
#include "../hpp/LogData.hpp"
#include "../hpp/ta.hpp"

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
}

const std::map<INTERVAL, ulong> time_map = 
            std::map<INTERVAL, ulong>({{INTERVAL::s1, sec},{INTERVAL::m1, min},{INTERVAL::m3, min*3},{INTERVAL::m5, min*5},{INTERVAL::m15, min*15},{INTERVAL::m30, min*30},
                                       {INTERVAL::h1, hour},{INTERVAL::h2, hour*2},{INTERVAL::h4, hour*4},{INTERVAL::h6, hour*6},{INTERVAL::h8, hour*8},{INTERVAL::h12, hour*12},
                                        {INTERVAL::d1, day},{INTERVAL::d3, day*3},{INTERVAL::w1, week},{INTERVAL::M1, month}});

void GodBot::Run(){

    std::string pub_key, sec_key, symbol, logs_folder, api_config_file;
    double take_profit, stop_loss;
    ulong log_time;
    INTERVAL timerframe;

    try{
        ptree_t tmp;
        auto append = [](ptree_t* src, ptree_t* dest){for(auto i : *src){dest->push_back(i);}};
        auto key_by_value = [](const std::string& value){for(auto i : base_api::time_intervals_){if(value == i.second) return i.first; return INTERVAL::m1;}};

        api_config_file = config_.get<std::string>("api_config_file");
        pub_key = config_.get<std::string>("public_key");
        sec_key = config_.get<std::string>("secret_key");
        api_->set_keys(std::make_pair(pub_key, sec_key));
        api_->set_cfg(api_config_file);
        
        timerframe = key_by_value(config_.get<std::string>("timeframe"));
        symbol = config_.get<std::string>("symbol");
        take_profit = 1 + config_.get<double>("take_profit_percentage")/100;
        stop_loss = 1 - config_.get<double>("stop_loss_percentage")/100;
        logs_folder = config_.count("logs_folder") ? config_.get<std::string>("logs_folder") : DEFAULT_LOGS_FOLDER;
        log_time = config_.get<ulong>("log_time");
        is_stdlog = config_.count("log_to_std") ? config_.get<bool>("log_to_std") : false;

    }
    catch(std::exception& err){
        std::cerr<<err.what()<<'\n';
    }

    double balance = 1000.;
    double coins = 0;
    double ema7=0, ema7_old=0, ema25=0, ema25_old=0, ema99=0;

    const ulong starttime = string_to_ptree(api_->get_server_time()).get<ulong>("serverTime");
    
    LogData log_data;
    json_data r = api_->get_kline(symbol, timerframe, 0, 0, 1000);
    auto pt = string_to_ptree(r);

    std::vector<Kline> vec;

    for(auto i : pt.get_child(""))
    {
        vec.push_back(Kline(i.second.get_child("")));
    }

    ema7 = EMA(7,vec,vec.size()-1);
    ema25 = EMA(25,vec,vec.size()-1);
    ema99 = EMA(99,vec,vec.size()-1);

    bool in_order = false;
    bool idle = true;
    auto old_balance = balance;
    double last_price = 0.;
    double min_price = 0.;

    auto last_kline = Kline(api_->get_kline(symbol, timerframe, 0, 0, 1));

    if(!std::filesystem::is_directory(logs_folder))
        std::filesystem::create_directory(logs_folder);

    std::string log_file_out;

    auto lasttime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
    auto idletime = lasttime;
    auto updatetime = lasttime;

    bool is_over = false;

    std::cout<<"> Bot "<<GetTag()<<" started traiding on "<<symbol<<"\n";
    log_file_out = logs_folder + '/' + std::to_string(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock().now().time_since_epoch()).count())+"_log.json";
    std::ofstream logs_out(log_file_out);
    logs_out<<"[\n"; // TODO: Fix log json-file write

    while(!is_finish)
    {
        auto curtime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
        
        if((curtime-lasttime).count() >= 1){
            last_kline = Kline(api_->get_kline(symbol, timerframe, 0, 0, 1));
            last_price = last_kline.get_close_price();
            min_price = last_kline.get_min_price();
            

            if((curtime-updatetime).count() >= time_map.at(timerframe)/sec){
                ema7_old = ema7;
                ema7=EMA(7, last_price, ema7);
                ema25_old = ema25;
                ema25=EMA(25, last_price, ema25);
                ema99=EMA(99, last_price, ema99);
                updatetime = curtime;
            }

            if(!in_order)
            {
                if(ema99>ema25){
                    if(ema25>ema7 && (ema99-ema25)/(ema25-ema7) >= 2.5 && ema7 > ema7_old && ema25>=ema25_old){
                        idletime = std::chrono::seconds(0);
                        in_order = true;
                        coins = balance/min_price*0.999;
                        balance = 0;
                        log_data.set(min_price, balance, coins, old_balance, ema7, ema25, ema99);
                        write_log(logs_out, log_data, ORDER_SIDE::BUY, is_stdlog);
                    }
                }
            }
            else if(coins*last_price>=old_balance*take_profit || coins*last_price<old_balance*stop_loss){
                        idletime = std::chrono::seconds(0);
                        in_order = false;
                        balance = coins*last_price*0.999;
                        coins = 0;
                        log_data.set(last_price, balance, coins, old_balance, ema7, ema25, ema99);
                        old_balance = balance;
                        write_log(logs_out, log_data, ORDER_SIDE::SELL, is_stdlog);
            }
        }

        if((curtime-idletime).count() >= log_time)
        {
            log_data.set(last_price, balance, coins, old_balance, ema7, ema25, ema99);
            write_log(logs_out, log_data, ORDER_SIDE::NONE, is_stdlog);
            idletime = curtime;
        }

        lasttime = curtime;
    }
    std::cout<<"> Bot "<<GetTag()<<" finished traiding on "<<symbol<<std::endl;
    logs_out<<"\n]"; // TODO: Fix log json-file write
    logs_out.close();
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
    // json_parser::write_json(std::cout,config_)
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