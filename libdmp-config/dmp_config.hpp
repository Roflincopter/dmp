#pragma once

#include <boost/property_tree/ptree.hpp>

namespace config {

typedef boost::property_tree::ptree array;

int get_volume();
void set_volume(int volume);

boost::property_tree::ptree get_library_information();
void add_library(std::string name, std::string path, std::string cache_file);

boost::property_tree::ptree get_servers();
void add_server(std::string name, std::string hostname, std::string port);
void clear_servers();

void print_config_dir();
void print_config_file_name();

}