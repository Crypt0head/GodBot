#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <filesystem>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/program_options.hpp>
#include <boost/exception/exception.hpp>

#include "../hpp/binance_api.hpp"
#include "../hpp/ta.hpp"
#include "../hpp/GodBot.hpp"

#define VERSION "0.2.0"

#define DEFAULT_CONFIG_FILE "cfg/config.json" 
#define DEFAULT_SECRETS_FILE "cfg/secrets.json" 
#define DEFAULT_API_FILE "cfg/binance_api.json" 

#define DEFAULT_LOGS_FOLDER "logs"

#define sec 1000
#define min sec*60
#define hour min*60
#define day hour*24
#define weak day*7
#define month waek*4

namespace opt = boost::program_options;
namespace filesystem = std::filesystem;

using ptree_t = boost::property_tree::ptree;

struct LogData{
    double last_price = 0;
    double balance = 0;
    double coins = 0;
    double old_balance = 0;
    double ema7 = 0;
    double ema25 = 0;
    double ema99 = 0;

    void set(const double& lp,const double& b, const double& c, const double& ob, const double& e7, const double& e25, const double& e99){
        last_price = lp;
        balance = b,
        coins = c;
        old_balance = ob;
        ema7 = e7;
        ema25 = e25;
        ema99 = e99;
    }
};

void write_log(std::ostream& os, LogData& data, ORDER_SIDE side = ORDER_SIDE::NONE, bool std_out = true){
    ptree_t root, logs, log;
    std::ostringstream strs;
    strs<<std::fixed<<std::setprecision(8);

    std::time_t time = ::time(nullptr);
    auto ltm = std::localtime(&time);
    auto log_ts = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock().now().time_since_epoch()).count();

    auto timestamp = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock().now().time_since_epoch()).count();

    log.put("timestamp", timestamp);
    log.put("local_time", std::put_time(ltm, "%c %Z"));

    if(side == ORDER_SIDE::BUY){
        log.put("status", "BUY");    
    }else if(side == ORDER_SIDE::SELL){
        log.put("status", "SELL");
        strs<<data.balance-data.old_balance;
        log.put("diff", strs.str()); strs.str("");
        strs<<data.balance/data.old_balance;
        log.put("gain", strs.str()); strs.str("");
    }else{
        log.put("status", "IDLE");    
    }

    strs<<data.last_price;
    log.put("last_price", strs.str()); strs.str("");
    strs<<data.balance;
    log.put("balance", strs.str()); strs.str("");
    strs<<data.coins;
    log.put("coins", strs.str()); strs.str("");
    strs<<data.coins*data.last_price;
    log.put("approximated_price", strs.str()); strs.str("");
    strs<<data.old_balance;
    log.put("old_balance", strs.str()); strs.str("");
    strs<<data.ema7;
    log.put("ema(7)", strs.str()); strs.str("");
    strs<<data.ema25;
    log.put("ema(25)", strs.str()); strs.str("");
    strs<<data.ema99;
    log.put("ema(99)", strs.str()); strs.str("");
    
    logs.push_back(std::make_pair("", log));

    try{
        root.push_back(std::make_pair(std::to_string(log_ts), log));
    }
    catch(const std::exception& e){
        std::cerr<<e.what()<<std::endl;
    }

    if(json_parser::verify_json(root, 0)){
         json_parser::write_json(os, root);
         if(std_out) 
            json_parser::write_json(std::cout, root);
    }
    else{
        throw json_parser::json_parser_error("Can't write ptree root to json-file: ", __FILE__, __LINE__);
    }
    os<<",\n";
}

int options_handler(opt::options_description* desc, opt::variables_map& vm, ptree_t* options){
    if(vm.count("help")){
         std::cout<< *desc <<std::endl;
         return 1;
    }
    if(vm.count("version")){
         std::cout<<"Version: "<<VERSION<<std::endl;
         return 1;
    }

    if(vm.count("config")){
        options->put("config_file", vm["config"].as<std::string>());

    }else{
        options->put("config_file", DEFAULT_CONFIG_FILE);
    }

    if(vm.count("secrets")){
        options->put("secrets_file", vm["secrets"].as<std::string>());
    }else{
        options->put("secrets_file", DEFAULT_SECRETS_FILE);
    }

    return 0;
}

int main(int argc, char** argv){
    opt::options_description description("All options");
    opt::variables_map vm;

    description.add_options()
    ("config,c", opt::value<std::string>()->default_value(DEFAULT_CONFIG_FILE), "path to GodBot config file")
    ("secrets,s", opt::value<std::string>()->default_value(DEFAULT_SECRETS_FILE), "path to secrets config file")
    ("version,v", "print version")
    ("help,h", "help message");

    try{
        opt::store(opt::parse_command_line(argc, argv, description), vm);
        opt::store(opt::parse_environment(description, "GODBOT_"), vm);
    }catch(const std::exception& e){
        std::cout<<e.what()<<std::endl;
        return 1;
    }
    opt::notify(vm);

    ptree_t config;
    if(options_handler(&description, vm, &config)){
        return 1;
    }

    std::string pub_key, sec_key, symbol, logs_folder, api_config_file;
    double take_profit, stop_loss;
    ulong log_time;
    bool log_to_std;
    INTERVAL timerframe;

    try{
        ptree_t tmp;
        auto append = [](ptree_t* src, ptree_t* dest){for(auto i : *src){dest->push_back(i);}};
        auto keybyvalue = [](const std::string& value){for(auto i : binance_api::time_intervals_){if(value == i.second) return i.first; return INTERVAL::m1;}};

        json_parser::read_json(config.get<std::string>("config_file") ,tmp);
        append(&tmp, &config);
        json_parser::read_json(config.get<std::string>("secrets_file") ,tmp);
        append(&tmp, &config);

        api_config_file = config.get<std::string>("api_config_file");
        pub_key = config.get<std::string>("public_key");
        sec_key = config.get<std::string>("secret_key");
        timerframe = keybyvalue(config.get<std::string>("timeframe"));
        symbol = config.get<std::string>("symbol");
        take_profit = 1 + config.get<double>("take_profit_percentage")/100;
        stop_loss = 1 - config.get<double>("stop_loss_percentage")/100;
        logs_folder = config.count("logs_folder") ? config.get<std::string>("logs_folder") : DEFAULT_LOGS_FOLDER;
        log_time = config.get<ulong>("log_time");
        log_to_std = config.count("log_to_std") ? config.get<bool>("log_to_std") : false;
    }
    catch(std::exception& err){
        std::cerr<<err.what()<<'\n';
        return 1;
    }

    binance_api api(pub_key, sec_key, api_config_file);

    double balance = 1000.;
    double coins = 0;
    double ema7=0, ema7_old=0, ema25=0, ema25_old=0, ema99=0;
    
    const ulong starttime = string_to_ptree(api.get_server_time()).get<ulong>("serverTime");

    std::vector<Kline> vec;
    
    LogData log_data;
    json_data r = api.get_kline(symbol, timerframe, 0, 0, 1000);
    auto pt = string_to_ptree(r);

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

    auto last_kline = Kline(api.get_kline(symbol, timerframe, 0, 0, 1));

    if(!std::filesystem::is_directory(logs_folder))
        std::filesystem::create_directory(logs_folder);

    std::string log_file_out;

    auto lasttime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
    auto idletime = lasttime;

    bool is_over = false;

    GodBot bot1;

    std::thread bot_thread([&](){

        std::cout<<"> Bot started traiding on "<<symbol<<"\n";
        log_file_out = logs_folder + '/' + std::to_string(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock().now().time_since_epoch()).count())+"_log.json";
        std::ofstream logs_out(log_file_out);
        logs_out<<"[\n"; // TODO: Fix log json-file write
        while(!is_over)
        {
            auto curtime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
            
            if((curtime-lasttime).count() >= 1){
                last_kline = Kline(api.get_kline(symbol, timerframe, 0, 0, 1));
                last_price = last_kline.get_close_price();
                min_price = last_kline.get_min_price();
                
                if((curtime-lasttime).count() >= log_time){
                    ema7_old = ema7;
                    ema7=EMA(7, last_price, ema7);
                    ema25_old = ema25;
                    ema25=EMA(25, last_price, ema25);
                    ema99=EMA(99, last_price, ema99);
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
                            write_log(logs_out, log_data, ORDER_SIDE::BUY, log_to_std);
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
                            write_log(logs_out, log_data, ORDER_SIDE::SELL, log_to_std);
                }
                lasttime = curtime;
            }

            if((curtime-idletime).count() >= log_time)
            {
                log_data.set(last_price, balance, coins, old_balance, ema7, ema25, ema99);
                write_log(logs_out, log_data, ORDER_SIDE::NONE, log_to_std);
                idletime = curtime;
            }
        }
        std::cout<<"> Bot finished traiding on "<<symbol<<std::endl;
        logs_out<<"\n]"; // TODO: Fix log json-file write
        logs_out.close();
    });

    char* cmd = new char[512];
    while(true)
    {
        std::cout<<"> ";
        std::cin.getline(cmd,512);

        if(!strcmp(cmd,"exit")){
            is_over = true;
            bot_thread.join();
            break;
        }

        if(!strcmp(cmd,"logs")){
            log_to_std = !log_to_std;
        }

    }

    delete[] cmd;
    return 0;
}