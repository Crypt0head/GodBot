#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include "../hpp/ta.hpp"

#define N 2

TEST_CASE("TA tests") {
    bool b = false;

    std::vector<Kline> data;

    srand(5);

    for(size_t i = 0; i < 50; ++i) {
        data.push_back(Kline(0, 0., 0., 0., rand() % 100, 0., 0., 0, 0., 0.));
        // data.push_back(Kline(0, 0., 0., 0., i + 1, 0., 0., 0, 0., 0.));
    }

    //std::reverse(data.begin(), data.end());

    auto print = [&](){for(auto i : data){std::cout << i.get_close_price() << ",";} std::cout<<std::endl;};

    std::cout << std::endl;

    SECTION("RSI") {

        print();

        auto rsi = RSI(N, data);

        std::cout << std::endl << "RSI: " << rsi << std::endl;

        if(rsi == 0.) {
            b = true;
        }

        REQUIRE(b);
    }

    std::cout << std::endl;

    SECTION("EMA") {

        if(EMA(2, 2, 1) == 1.667) {
            b = true;
        }

        REQUIRE(b);
    }
}