#define CATCH_CONFIG_MAIN

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/exception/exception.hpp>
#include <boost/iostreams/stream.hpp>

#include "catch.hpp"
#include "../hpp/binance_api.hpp"

namespace json_parser = boost::property_tree::json_parser;
namespace iostreams = boost::iostreams;

using ptree_t = boost::property_tree::ptree;

// TEST_CASE("Equalution test for USD and CHF")
// {
//     Dollar usd(12);
//     Frank chf(5);
//     bool b = false;

//     SECTION("12 USD == 12 Money(USD)"){
//         b = usd==Money(12, Currency::USD);
//         REQUIRE(b == true);
//     }

//     SECTION("12 USD != 12 CHF"){
//         b = usd==Money(12,Currency::CHF);
//         REQUIRE(b == false);
//     }

//     SECTION("5 CHF == 5 Money(CHF)"){
//         b = chf==Money(5, Currency::CHF);
//         REQUIRE(b == true);
//     }

//     SECTION("5 CHF != 5 USD"){
//         b = chf==Money(5,Currency::USD);
//         REQUIRE(b == false);
//     }

//     SECTION("5 CHF != 12 USD"){
//         b = chf==usd;
//         REQUIRE(b == false);
//     }
// }

ptree_t cfg;

TEST_CASE("General Tests"){
    bool b = false;

    SECTION("Config file exists"){
        try{ 
            json_parser::read_json("../cfg/secrets.json",cfg);
            b = true;
        }
        catch(boost::wrapexcept<json_parser::json_parser_error> &err){
            std::cerr<<err.what()<<'\n';
        }

        b = b && (cfg.get<bool>("test") == true);

        REQUIRE(b);
    }

    SECTION("Config file doesn't exist"){
        try{ 
            json_parser::read_json("../cfg/secrets_1.json",cfg);
            b = true;
        }
        catch(boost::wrapexcept<json_parser::json_parser_error> &err){
        }

        REQUIRE(b == false);
    }

    try{ 
        json_parser::read_json("../cfg/secrets.json",cfg);
    }
    catch(boost::wrapexcept<json_parser::json_parser_error> &err){
        std::cerr<<err.what()<<'\n';
    }
}

ulong orderId = 0;

TEST_CASE("SPOT TRAIDING"){
    bool b = false;

    SECTION("TEST ORDER"){
        std::string key = cfg.get<std::string>("public_key");
        std::string sec = cfg.get<std::string>("secret_key");

        binance_api api(key,sec);

        try{
            auto res = api.call("/order/test",api.build({"symbol=VETUSDT","side=BUY","type=LIMIT","timeInForce=GTC","quantity=700","price=0.015", "recvWindow=60000"}),http::REQTYPE::POST);
            b = !static_cast<bool>(res.compare("{}"));
        }
        catch(std::exception &e){
            std::cerr<<e.what()<<std::endl;
        }

        REQUIRE(b);
    }

    SECTION("SET SPOT ORDER"){
        std::string key = cfg.get<std::string>("public_key");
        std::string sec = cfg.get<std::string>("secret_key");

        binance_api api(key,sec);
        ptree_t res;

        try{
            //auto str = api.call("/order",api.build({"symbol=VETUSDT","side=BUY","type=LIMIT","timeInForce=GTC","quantity=700","price=0.015"}),http::REQTYPE::POST);
            auto str = api.open_spot_order("VETUSDT",ORDER_SIDE::BUY,ORDER_TYPE::LIMIT,700.,0.015);
            iostreams::array_source as(&str[0],str.size());
            iostreams::stream<iostreams::array_source> is(as);
            json_parser::read_json(is,res);
            
            try{
                orderId = res.get<ulong>("orderId");
                b = true;
            }
            catch(std::exception &e){
                std::cerr<<e.what()<<std::endl<<str<<std::endl;
            }
        }
        catch(std::exception &e){
            std::cerr<<e.what()<<std::endl;
        }

        REQUIRE(b);
    }

    SECTION("CANCEL SPOT ORDER"){
        std::string key = cfg.get<std::string>("public_key");
        std::string sec = cfg.get<std::string>("secret_key");

        binance_api api(key,sec);
        ptree_t res;

        try{
            auto str = api.call("/order",api.build({"symbol=VETUSDT", "orderId=" + std::to_string(orderId), "recvWindow=60000"}),http::REQTYPE::DELETE);
            iostreams::array_source as(&str[0],str.size());
            iostreams::stream<iostreams::array_source> is(as);
            json_parser::read_json(is,res);
            
            try{
                    b = !static_cast<bool>(res.get<std::string>("status").compare("CANCELED"));
            }
            catch(std::exception &e){
                std::cerr<<e.what()<<std::endl;
            }
        }
        catch(std::exception &e){
            std::cerr<<e.what()<<std::endl;
        }

        REQUIRE(b);
    }
}