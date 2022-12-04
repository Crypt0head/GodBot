#include "../hpp/GodBot.hpp"
#include "../hpp/base_api.hpp"

std::vector<std::string> tags = {"alice", "bob", "robert", "nik", "sam", "jeth", "roger", "albert", "hans", "jim", "warg", "orm", "aki", "egel", "skalagrim"};

std::string GenerateTag(){
    srand(::time(nullptr));

    std::string postfix = std::to_string(rand()%1000);

    return tags[rand()%tags.size()] + '_' + postfix;

}

uint64_t GodBot::count_ = 0;

GodBot::GodBot(const std::string& tag = ""){
    start_time_ = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock().now().time_since_epoch());
    id_ = count_++;
    SetTag(tag);
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

void GodBot::SetConfig(ptree_t& cfg){
    config_ = cfg;
}

const ptree_t& GodBot::GetConfig() const{
    return config_;
};

void GodBot::UploadConfig(const ptree_t& config){
    config_ = config;

    try{
        ptree_t tmp;
        auto append = [](ptree_t* src, ptree_t* dest){for(auto i : *src){dest->push_back(i);}};

        json_parser::read_json(config_.get<std::string>("config_file") ,tmp);
        append(&tmp, &config_);
        json_parser::read_json(config_.get<std::string>("secrets_file") ,tmp);
        append(&tmp, &config_);
    }
    catch(std::exception& e)
    {
        std::cerr<<e.what()<<std::endl;
    }
}

void GodBot::SetTag(const std::string& tag = ""){
    
    if(tag.size() == 0){
        tag_ = GenerateTag();
        return;
    }

    tag_ = tag;
}

const std::string& GodBot::GetTag() const{
    return tag_;
}

GodBot::~GodBot(){
    count_--;
}