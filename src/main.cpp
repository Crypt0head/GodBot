#include <iostream>
#include <fstream>
#include <sstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/exception/exception.hpp>
#include <thread>

#include "../hpp/binance_api.hpp"
#include "../hpp/ta.hpp"

#define CONFIGFILE "../cfg/secrets.json" 
#define LOGS_FOLDER "../logs/"

#define sec 1000
#define min sec*60
#define hour min*60
#define day hour*24
#define weak day*7
#define month waek*4

using ptree_t = boost::property_tree::ptree;

struct LogData{
    double latest_price = 0;
    double balance = 0;
    double coins = 0;
    double old_balance = 0;
    double ema7 = 0;
    double ema25 = 0;
    double ema99 = 0;

    void set(const double& lp,const double& b, const double& c, const double& ob, const double& e7, const double& e25, const double& e99){
        latest_price = lp;
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

    strs<<data.latest_price;
    log.put("latest_price", strs.str()); strs.str("");
    strs<<data.balance;
    log.put("balance", strs.str()); strs.str("");
    strs<<data.coins;
    log.put("coins", strs.str()); strs.str("");
    strs<<data.coins*data.latest_price;
    log.put("approximated_price", strs.str()); strs.str("");
    strs<<data.old_balance;
    log.put("old_balance", strs.str()); strs.str("");
    strs<<data.ema7;
    log.put("ema(7)", strs.str()); strs.str("");
    strs<<data.ema25;
    log.put("ema(25)", strs.str()); strs.str("");
    strs<<data.ema99;
    log.put("ema(99)", strs.str()); strs.str("");
    
    logs.push_back(std::make_pair("",log));

    try{
        root.push_back(std::make_pair(std::to_string(log_ts),log));
    }
    catch(const std::exception& e){
        std::cerr<<e.what()<<std::endl;
    }

    os.precision(8); os.fixed;
    std::cout.precision(8); std::cout.fixed;

    if(json_parser::verify_json(root,0)){
         json_parser::write_json(os,root);
         if(std_out) 
            json_parser::write_json(std::cout,root);
    }
    else{
        throw json_parser::json_parser_error("Can't write ptree root to json-file: ", __FILE__, __LINE__);
    }
    os<<",\n";
}

int main(int argc, char** argv){
    ptree_t config;
    std::string log_file_out = LOGS_FOLDER + std::to_string(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock().now().time_since_epoch()).count())+"_log.json";
    std::ofstream logs_out(log_file_out);
    std::string pub_key, sec_key;

    logs_out<<"[\n"; // TODO: Fix log json-file write

    try{ 
        boost::property_tree::json_parser::read_json(CONFIGFILE,config);
    }
    catch(boost::wrapexcept<boost::property_tree::json_parser_error> &err){
        std::cerr<<err.what()<<'\n';
        return 1;
    }

    pub_key = config.get<std::string>("public_key");
    sec_key = config.get<std::string>("secret_key");

    binance_api api(pub_key,sec_key);

    double balance = 1000.;
    double coins = 0;
    double ema7=0, ema7_old=0, ema25=0, ema25_old=0, ema99=0;
    
    const ulong starttime = string_to_ptree(api.get_server_time()).get<ulong>("serverTime");

    std::vector<Kline> vec;
    
    LogData log_data;
    json_data r = api.get_kline("LUNCUSDT",INTERVAL::m1,0,0,1000);
    auto pt = string_to_ptree(r);

    for(auto i : pt.get_child(""))
    {
        vec.push_back(Kline(i.second.get_child("")));
    }

    ema7 = EMA(7,vec,vec.size()-1);
    ema25 = EMA(25,vec,vec.size()-1);
    ema99 = EMA(99,vec,vec.size()-1);

    auto lasttime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
    auto idletime = lasttime;

    bool in_order = false;
    bool idle = true;
    auto old_balance = balance;
    double latest_price = 0.;

    while(true)
    {
        auto curtime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
        
        if((curtime-lasttime).count() >= 60){
            latest_price = Kline(api.get_kline("LUNCUSDT",INTERVAL::m1,0,0,1)).get_close_price();
            
            ema7_old = ema7;
            ema7=EMA(7,vec,ema7);
            ema25_old = ema25;
            ema25=EMA(25,vec,ema25);
            ema99=EMA(99,vec,ema99);

            std::time_t time = ::time(nullptr);
            auto ltm = std::localtime(&time);

            if(!in_order)
            {
                if(ema99>ema25){
                    if(ema25>ema7 && (ema99-ema25)/(ema25-ema7) >= 2.5 && ema7 > ema7_old && ema25>ema25_old){
                        idletime = std::chrono::seconds(0);
                        in_order = true;
                        coins = (float)(balance/latest_price*0.999);
                        balance = 0;
                        log_data.set(latest_price, balance, coins, old_balance, ema7, ema25, ema99);
                        write_log(logs_out,log_data,ORDER_SIDE::BUY);
                    }
                }
            }
            else if(coins*latest_price>=old_balance*1.0031){
                        idletime = std::chrono::seconds(0);
                        in_order = false;
                        balance = coins*latest_price*0.999;
                        coins = 0;
                        log_data.set(latest_price, balance, coins, old_balance, ema7, ema25, ema99);
                        old_balance = balance;
                        write_log(logs_out,log_data,ORDER_SIDE::SELL);
            }
            lasttime = curtime;
        }

        if((curtime-idletime).count() >= 60)
        {
            log_data.set(latest_price, balance, coins, old_balance, ema7, ema25, ema99);
            write_log(logs_out,log_data);
            idletime = curtime;
        }
    }
    
    logs_out<<"\n]"; // TODO: Fix log json-file write
    logs_out.close();

    return 0;
}