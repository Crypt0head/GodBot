#include "../hpp/LogData.hpp"
#include "../hpp/base_api.hpp"

void LogData::set(const double& lp,const double& b, const double& c, const double& ob, const double& e7, const double& e25, const double& e99){
    last_price = lp;
    balance = b,
    coins = c;
    old_balance = ob;
    ema7 = e7;
    ema25 = e25;
    ema99 = e99;
}

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