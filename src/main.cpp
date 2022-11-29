#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/exception/exception.hpp>

#include "../hpp/binance_api.hpp"
#include "../hpp/ta.hpp"

#define CONFIGFILE "../cfg/secrets.json" 

using ptree_t = boost::property_tree::ptree;



int main(int argc, char** argv){

    std::vector<double> data;
    TI_REAL d[50], dout[50];

    srand(::time(nullptr));

    for(int i=0;i<50;i++)
    {
        double v = rand()%99;
        data.push_back(v);
        d[i]=v;
        std::cout<<data[data.size()-1]<<",";
    }

    ti_ema(50,d,7,dout);
    std::cout<<"\n\n"<<EMA(7,data,49)<<std::endl;
    std::cout<<dout[49]<<std::endl;

    ptree_t config;
    std::string pub_key, sec_key;

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


    json_data r = api.get_kline("LUNCUSDT",INTERVAL::m1);
    Kline kl(r);

 
    return 0;
}