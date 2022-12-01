#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/exception/exception.hpp>
#include <thread>

#include "../hpp/binance_api.hpp"
#include "../hpp/ta.hpp"

#define CONFIGFILE "../cfg/secrets.json" 

#define sec 1000
#define min sec*60
#define hour min*60
#define day hour*24
#define weak day*7
#define month waek*4

using ptree_t = boost::property_tree::ptree;



int main(int argc, char** argv){
    ptree_t config;
    std::string pub_key, sec_key;
    bool isOnline = true;

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
    
    // ulong time = string_to_ptree(api.get_kline("LUNCUSDT",INTERVAL::m5,0,0,1)).get_child(".").begin()->second.get<ulong>("");// starttime/10000*10000-min;
    
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

    while(true)
    {
        auto curtime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
        
        if((curtime-lasttime).count() >= 60){
            vec.push_back(Kline(api.get_kline("LUNCUSDT",INTERVAL::m1,0,0,1)));
            std::cout<<"> Getting latest price: "<<vec.back().get_close_price()<<"\n";
            
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
                        std::cout<<"> ["<<std::put_time(ltm, "%c %Z")<<"]: "<<"Open BUY order: "<<"\n";
                        in_order = true;
                        coins = (float)(balance/vec.back().get_close_price()*0.999);
                        balance = 0;
                        std::cout<<"> Get: "<<coins<<" coins on price: "<<vec.back().get_close_price()<<"\n";
                    }
                }
            }
            else if(coins*vec.back().get_close_price()>=old_balance*1.0031){
                        idletime = std::chrono::seconds(0);
                        std::cout<<"> ["<<std::put_time(ltm, "%c %Z")<<"]: Open SELL order: "<<"\n";
                        in_order = false;
                        balance = coins*vec.back().get_close_price()*0.999;
                        coins = 0;
                        std::cout<<"> Old balance: "<<old_balance<<" New balance: "<<balance<<" $ on price: "<<vec.back().get_close_price()<<"\n";
                        std::cout<<"> Diff: "<<balance-old_balance<<". Gain: "<<balance/old_balance*100 - 100<<"%\n";
                        old_balance = balance;
            }
            lasttime = curtime;
        }

        if((curtime-idletime).count() >= 60)
        {
            std::time_t time = ::time(nullptr);
            auto ltm = std::localtime(&time);
            std::cout<<"> ["<<std::put_time(ltm, "%c %Z")<<"]: \n";
            auto apr = [&in_order, &coins, &vec](){ return in_order ? "(~"+ std::to_string(coins*vec.back().get_close_price()) + " $)" : std::string("");};
            std::cout<<"Status: \n\tbalance: "<<balance<<"\n\tcoins: "<<coins<<apr()<<"\n\t"<<"old_balance: "<<old_balance<<"\n\t";
            std::cout<<"EMA(7)= "<<ema7<<"\n\t";
            std::cout<<"EMA(25)= "<<ema25<<"\n\t";
            std::cout<<"EMA(99)= "<<ema99<<"\n";
            idletime = curtime;
        }
    }
 
    return 0;
}