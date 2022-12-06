#include <filesystem>

#include "../hpp/GB_Logger.hpp"
#include "../hpp/GB_LogData.hpp"

GB_Logger::GB_Logger(){}

GB_Logger::GB_Logger(const base_data* data){
    data_ = data->clone();
}

void GB_Logger::set_log_stream(std::ostream& s){
    os_ = &s;
}

void GB_Logger::set_log_file(const std::string& filename){
    os_ = new std::ofstream(log_folder_ + filename); // TODO: AVOID memory leak!
}

std::ostream& GB_Logger::get_log_stream(){
    return *os_;
}

void GB_Logger::set_log_folder(const std::string& folder){
    if(folder.back() == '/')
        log_folder_ = folder;
    else
        log_folder_ = folder + '/';
    
    if(!std::filesystem::is_directory(log_folder_))
        std::filesystem::create_directory(log_folder_);
}

const std::string& GB_Logger::get_log_folder() const{
    return log_folder_;
}

void GB_Logger::output(const ORDER_SIDE& side) const{

    data_->write_log(*os_, side);
}

void GB_Logger::set_log_data(const double& lp,const double& b, const double& c, const double& ob, const double& e7, const double& e25, const double& e99){
    data_->set(lp,b,c,ob,e7,e25,e99);
}

GB_Logger::~GB_Logger(){
    
}