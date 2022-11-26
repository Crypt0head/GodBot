#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/exception/exception.hpp>

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

TEST_CASE("General Tests"){
    bool b = false;
    ptree_t cfg;

    SECTION("Config file exists"){
        try{ 
            boost::property_tree::json_parser::read_json("../cfg/secrets.json",cfg);
            b = true;
        }
        catch(boost::wrapexcept<boost::property_tree::json_parser_error> &err){
            std::cerr<<err.what()<<'\n';
        }

        b = b && (cfg.get<bool>("test") == true);

        REQUIRE(b);
    }

    SECTION("Config file doesn't exist"){
        try{ 
            boost::property_tree::json_parser::read_json("../cfg/secrets_1.json",cfg);
            b = true;
        }
        catch(boost::wrapexcept<boost::property_tree::json_parser_error> &err){
            std::cerr<<err.what()<<'\n';
        }

        REQUIRE(b == false);
    }
}