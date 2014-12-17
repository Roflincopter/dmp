#pragma once

#include <boost/property_tree/ptree.hpp>

boost::optional<boost::property_tree::ptree&> get_library_information();
void config_add_library(std::string name, std::string path, std::string cache_file);

void print_config_dir();
void print_config_file_name();
