#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/exception/exception.hpp>

#include "../hpp/binance_api.hpp"

#define CONFIGFILE "../cfg/secrets.json" 

using ptree_t = boost::property_tree::ptree;

int main(int argc, char** argv){

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
    json_data response = api.call("/order",api.build({"symbol=VETUSDT","side=BUY","type=LIMIT","timeInForce=GTC","quantity=700","price=0.015"}));

    std::cout<<response<<std::endl;
 
    return 0;
}