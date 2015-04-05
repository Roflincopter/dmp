#pragma once

#include <boost/fusion/adapted/struct.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/filesystem/path.hpp>

#include <vector>

namespace config {

struct ServerInfo {
	std::string name;
	std::string hostname;
	std::string port;
};

struct LibraryInfo {
	std::string name;
	std::string path;
	std::string cache_file;
};

boost::filesystem::path get_config_file_name();
boost::filesystem::path get_database_file_name();
boost::filesystem::path get_library_folder_name();

int get_volume();
void set_volume(int volume);

std::vector<LibraryInfo> get_library_information();
void add_library(std::string name, std::string path, std::string cache_file);

std::vector<ServerInfo> get_servers();
void add_server(std::string name, std::string hostname, std::string port);
void clear_servers();

void print_config_dir();
void print_config_file_name();

}
