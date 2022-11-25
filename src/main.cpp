#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/exception/exception.hpp>

#include "../lib/exmo-api-lib/exmo_api.hpp"

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

    exmo_api api(pub_key,sec_key);
    json_data response = api.call("user_info", "");

    std::cout<<"User Info:\n"<<response<<'\n';
 
    return 0;
}