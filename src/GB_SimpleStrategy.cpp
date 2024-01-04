// #include <boost/property_tree/ptree.hpp>
// #include <boost/property_tree/json_parser.hpp>

// #include "../hpp/base_api.hpp"
// #include "../hpp/GB_LogData.hpp"
// #include "../hpp/GB_Logger.hpp"
// #include "../hpp/GB_SimpleStrategy.hpp"

// namespace chrono = std::chrono;
// namespace json_parser = boost::property_tree::json_parser;

// using ptree_t = boost::property_tree::ptree;

// #define sec 1000
// #define min sec*60
// #define hour min*60
// #define day hour*24
// #define week day*7
// #define month (ulong)week*4

// static const std::map<INTERVAL, ulong> time_map = 
//             std::map<INTERVAL, ulong>({{INTERVAL::s1, sec},{INTERVAL::m1, min},{INTERVAL::m3, min*3},{INTERVAL::m5, min*5},{INTERVAL::m15, min*15},{INTERVAL::m30, min*30},
//                                        {INTERVAL::h1, hour},{INTERVAL::h2, hour*2},{INTERVAL::h4, hour*4},{INTERVAL::h6, hour*6},{INTERVAL::h8, hour*8},{INTERVAL::h12, hour*12},
//                                         {INTERVAL::d1, day},{INTERVAL::d3, day*3},{INTERVAL::w1, week},{INTERVAL::M1, month}});

// void GB_SimpleStrategy::Run(){
//     double balance = 1000.;
//     double coins = 0;
//     double ema7=0, ema7_old=0, ema25=0, ema25_old=0, ema99=0;

//     const ulong starttime = string_to_ptree(api_->get_server_time()).get<ulong>("serverTime");

//     json_data r = api_->get_kline(symbol_, timerframe_, 0, 0, 1000);
//     auto pt = string_to_ptree(r);

//     std::vector<Kline> vec;

//     for(auto i : pt.get_child(""))
//     {
//         vec.push_back(Kline(i.second.get_child("")));
//     }

//     // ema7 = EMA(7,vec,vec.size()-1);
//     // ema25 = EMA(25,vec,vec.size()-1);
//     // ema99 = EMA(99,vec,vec.size()-1);

//     double rsi14 = RSI(14, vec);

//     bool in_order = false;
//     bool idle = true;
//     auto old_balance = balance;
//     double last_price = 0.;
//     double min_kline_price = 0.;

//     auto last_kline = Kline(api_->get_kline(symbol_, timerframe_, 0, 0, 1));

//     auto lasttime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
//     auto idletime = lasttime;
//     auto updatetime = lasttime;

//     double max_price = string_to_ptree(api_->get_symbol_price(symbol_)).get<double>("price");

//     while(!*is_finished_)
//     {
//         auto curtime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
        
//         if((curtime-lasttime).count() >= 1){
//             last_kline = Kline(api_->get_kline(symbol_, timerframe_, 0, 0, 1));
//             last_price = last_kline.get_close_price();
//             min_kline_price = last_kline.get_min_price();
//             max_price = last_price > max_price ? last_price : max_price;

//             vec.push_back(last_kline);

//             if((curtime-updatetime).count() >= time_map.at(timerframe_)/sec){
//                 // ema7_old = ema7;
//                 // ema7=EMA(7, last_price, ema7);
//                 // ema25_old = ema25;
//                 // ema25=EMA(25, last_price, ema25);
//                 // ema99=EMA(99, last_price, ema99);

//                 rsi14 = RSI(14, vec);
                
//                 // std::cout << "Last value: " << vec[vec.size() - 1].get_close_price() << std::endl; 

//                 updatetime = curtime;
//             }

//             if(!in_order)
//             {
//                 // if(ema99>ema25){
//                 //     if(ema25>ema7 && (ema99-ema25)/(ema25-ema7) >= 3 && ema7 > ema7_old && ema25>=ema25_old && (max_price - last_price >= max_price*0.011)){
//                 //         idletime = std::chrono::seconds(0);
//                 //         in_order = true;
//                 //         coins = balance/min_kline_price*0.999;
//                 //         balance = 0;
//                 //         logger_->set_log_data(min_kline_price, balance, coins, old_balance, ema7, ema25, ema99);
//                 //         logger_->output(ORDER_SIDE::BUY);
//                 //     }
//                 // }
                
//                 if(rsi14 < 0.3) {
//                     idletime = std::chrono::seconds(0);
//                     in_order = true;
//                     coins = balance / min_kline_price * 0.999;
//                     balance = 0;
//                     logger_->set_log_data(min_kline_price, balance, coins, old_balance, rsi14, 0., 0.);
//                     logger_->output(ORDER_SIDE::BUY);
//                 }

//             }
//             else if(coins * last_price >= old_balance * take_profit_ || coins * last_price < old_balance * stop_loss_){
//                         idletime = std::chrono::seconds(0);
//                         in_order = false;
//                         balance = coins * last_price * 0.999;
//                         max_price = last_price;
//                         coins = 0;
//                         logger_->set_log_data(last_price, balance, coins, old_balance, rsi14, 0., 0.);
//                         old_balance = balance;
//                         logger_->output(ORDER_SIDE::SELL);
//             }
//         }

//         if((curtime-idletime).count() >= log_time_)
//         {
//             logger_->set_log_data(last_price, balance, coins, old_balance, rsi14, 0., 0.);
//             logger_->output(ORDER_SIDE::NONE);
//             idletime = curtime;

//             // std::cout << "RSI(14): " << rsi14 << std::endl;
//         }

//         lasttime = curtime;
//     }
// }