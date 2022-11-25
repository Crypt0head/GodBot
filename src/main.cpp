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

    // binance_api api(pub_key,sec_key);
    // json_data response = api.call("/order/test?symbol=VETUSDT",api.build({"side=BUY","type=LIMIT", "recWindow=60000"}));

    #include <string>
    #include "../hpp/hmac_sha256.hpp"

    http::connection con;
    headers_t headers; headers["X-MBX-APIKEY"] = pub_key;

    auto param = "symbol=VETUSDT&side=BUY&type=LIMIT&timeInForce=GTC&quantity=700&price=0.015&recvWindow=60000&timestamp=" + std::to_string((unsigned long)::time(nullptr)*1000);

    param += "&signature=" + HMAC_SHA256(sec_key, param).hex_digest();

    con.request("https://api.binance.com/api/v3/order/test?",http::post(),param,headers,http::REQTYPE::POST);

    std::string response = con.get_response();

    std::cout<<response<<std::endl;
 
    return 0;
}