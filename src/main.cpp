#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <csignal>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/program_options.hpp>
#include <boost/exception/exception.hpp>

#include "../hpp/binance_api.hpp"
#include "../hpp/GodBot.hpp"

#define VERSION "0.5.5"

#define DEFAULT_CONFIG_FILE "cfg/config.json" 
#define DEFAULT_SECRETS_FILE "cfg/secrets.json" 
#define DEFAULT_API_FILE "/cfg/binance_api.json" 

#define DEFAULT_LOGS_FOLDER "logs"

#define sec 1000
#define min sec*60
#define hour min*60
#define day hour*24
#define week day*7
#define month (ulong)week*4

namespace opt = boost::program_options;
namespace filesystem = std::filesystem;

using ptree_t = boost::property_tree::ptree;

int options_handler(opt::options_description* desc, opt::variables_map& vm, ptree_t* options){
    if(vm.count("help")){
         std::cout<< *desc <<std::endl;
         return 1;
    }
    if(vm.count("version")){
         std::cout<<"Version: "<<VERSION<<std::endl;
         return 1;
    }

    if(vm.count("config")){
        options->put("config_file", vm["config"].as<std::string>());

    }else{
        options->put("config_file", DEFAULT_CONFIG_FILE);
    }

    if(vm.count("secrets")){
        options->put("secrets_file", vm["secrets"].as<std::string>());
    }else{
        options->put("secrets_file", DEFAULT_SECRETS_FILE);
    }

    return 0;
}

int main(int argc, char** argv){ 
    
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGPIPE);
    if (pthread_sigmask(SIG_BLOCK, &set, NULL) != 0)
        return -1;

    opt::options_description description("All options");
    opt::variables_map vm;

    description.add_options()
    ("config,c", opt::value<std::string>()->default_value(DEFAULT_CONFIG_FILE), "path to GodBot config file")
    ("secrets,s", opt::value<std::string>()->default_value(DEFAULT_SECRETS_FILE), "path to secrets config file")
    ("version,v", "print version")
    ("help,h", "help message");

    try{
        opt::store(opt::parse_command_line(argc, argv, description), vm);
        opt::store(opt::parse_environment(description, "GODBOT_"), vm);
    }catch(const std::exception& e){
        std::cout<<e.what()<<std::endl;
        return 1;
    }
    opt::notify(vm);

    ptree_t config;
    if(options_handler(&description, vm, &config)){
        return 1;
    }

    GodBot bot1(""); 
    bot1.SetAPI(new binance_api);
    bot1.UploadConfig(config);

    std::thread bot_thread([&bot1](){bot1.Run();});

    char* cmd = new char[512];
    while(true)
    {
        std::cout<<"> ";
        std::cin.getline(cmd,512);

        if(!strcmp(cmd,"exit")){
            bot1.Stop();
            bot_thread.join();
            break;
        }

        if(!strcmp(cmd,"logs")){
            bot1.SwitchLog();
        }

        if(!strcmp(cmd,"cfg")){
            json_parser::write_json(std::cout, bot1.GetConfig());
        }

        if(!strcmp(cmd,"version")){
            std::cout<<"GodBot version: "<<VERSION<<std::endl;;
        }        

    }

    delete[] cmd;
    return 0;
}