#define CATCH_CONFIG_MAIN

#include <boost/exception/exception.hpp>

#include "catch.hpp"
#include "../hpp/binance_api.hpp"

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

TEST_CASE("MARKET DATA"){
    bool b = false;

    SECTION("GET MARKET DATA"){
        std::string key = cfg.get<std::string>("public_key");
        std::string sec = cfg.get<std::string>("secret_key");

        binance_api api(key,sec);
        ptree_t res;

        try{
            auto str = api.get_symbol_price("LUNCBUSD");
            res = string_to_ptree(str);

            b = !static_cast<bool>(res.get<std::string>("symbol").compare("LUNCBUSD"));
        }
        catch(std::exception &e){
            std::cerr<<e.what()<<std::endl;

        }

        REQUIRE(b);
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
            auto res = api.call("/order/test",api.build({"symbol=VETUSDT","side=BUY","type=LIMIT","timeInForce=GTC","quantity=1000","price=0.015", "recvWindow=60000"}),http::REQTYPE::POST, SECURITY_TYPE::SIGNED);
            b = !static_cast<bool>(res.compare("{}"));
        }
        catch(std::exception &e){
            std::cerr<<e.what()<<std::endl;
        }

        REQUIRE(b);
    }

    SECTION("OPEN SPOT ORDER"){
        std::string key = cfg.get<std::string>("public_key");
        std::string sec = cfg.get<std::string>("secret_key");

        binance_api api(key,sec);
        ptree_t res;
        ptree_t price = string_to_ptree(api.get_symbol_price("VETUSDT"));

        try{
            auto sym_price = price.get<double>("price");

            ulong q = 13/sym_price*0.92;
            auto p = std::round(sym_price*0.92*100000)/100000;

            auto str = api.open_spot_order("VETUSDT",ORDER_SIDE::BUY,ORDER_TYPE::LIMIT,q,p);
            res = string_to_ptree(str);
            
            
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
            std::string str = api.close_spot_order("VETUSDT",orderId);
            res = string_to_ptree(str);
            
            try{
                    b = !static_cast<bool>(res.get<std::string>("status").compare("CANCELED"));
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

    SECTION("CANCEL ALL SPOT ORDERS"){
        std::string key = cfg.get<std::string>("public_key");
        std::string sec = cfg.get<std::string>("secret_key");

        binance_api api(key,sec);
        ptree_t res;
        ptree_t price = string_to_ptree(api.get_symbol_price("VETBUSD"));
        auto sym_price = price.get<double>("price");

        ulong q = 13/sym_price*0.92;
        auto p = std::round(sym_price*0.92*100000)/100000;

        api.open_spot_order("VETBUSD",ORDER_SIDE::BUY,ORDER_TYPE::LIMIT,q,p);

        try{
            auto str = api.close_all_spot_orders("VETBUSD");
            res = string_to_ptree(str);
            
            try{
                bool tmp = true;
                for(auto& [k,n] : res.get_child(""))
                {
                    tmp &= !static_cast<bool>(n.get<std::string>("status").compare("CANCELED"));
                }
                b = tmp;
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

    SECTION("OPEN OCO SPOT ORDER"){
        std::string key = cfg.get<std::string>("public_key");
        std::string sec = cfg.get<std::string>("secret_key");

        binance_api api(key,sec);
        ptree_t res;
        ptree_t price = string_to_ptree(api.get_symbol_price("VETUSDT"));

        try{
                auto sym_price = price.get<double>("price");
                ulong q = 13/sym_price*0.92;
                auto p = std::round(sym_price*0.92*100000)/100000;
                auto sp = std::round(sym_price*1.05*100000)/100000;
                auto lp = std::round(sym_price*1.02*100000)/100000;

                auto str = api.open_oco_spot_order("VETBUSD",ORDER_SIDE::BUY,q,p,sp,lp);
                res = string_to_ptree(str);
                
                try{
                    orderId = res.get<ulong>("orderListId");
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

    SECTION("CANCEL OCO SPOT ORDER"){
        std::string key = cfg.get<std::string>("public_key");
        std::string sec = cfg.get<std::string>("secret_key");

        binance_api api(key,sec);
        ptree_t res;

        try{
            auto str = api.close_oco_spot_order("VETBUSD",orderId);
            res = string_to_ptree(str);
            
            try{
                b = !static_cast<bool>(res.get<std::string>("listOrderStatus").compare("ALL_DONE"));
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
};