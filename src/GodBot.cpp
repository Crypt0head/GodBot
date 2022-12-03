#include "../hpp/GodBot.hpp"

uint64_t GodBot::count_ = 0;

GodBot::GodBot(){
    start_time_ = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock().now().time_since_epoch());
    id_ = count_++;
}

void GodBot::Run(){
    std::cout<<"Bot Run"<<std::endl;
}

const uint64_t GodBot::GetID() const {
    return id_;
}

const chrono::milliseconds GodBot::GetLifeTime() const {
    auto cur_time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock().now().time_since_epoch());
    return cur_time-start_time_;
}

void GodBot::Pause(){
    is_pause = true;
}

void GodBot::Resume(){
    is_pause = false;
}

void GodBot::Stop(){
    

    GodBot::~GodBot();
}

const uint64_t GodBot::Count(){
    return count_;
}

GodBot::~GodBot(){
    count_--;
}