#pragma once

#include <iostream>
#include <vector>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/iostreams/stream.hpp>

namespace iostreams = boost::iostreams;
namespace json_parser = boost::property_tree::json_parser;

using ptree_t = boost::property_tree::ptree;

ptree_t string_to_ptree(const std::string& str){
		ptree_t res;
	    iostreams::array_source as(&str[0],str.size());
        iostreams::stream<iostreams::array_source> is(as);
		try{
        	json_parser::read_json(is, res);
		}
		catch(std::exception& e){
			std::cerr<<e.what()<<std::endl;
		}
		
		return res;
}

