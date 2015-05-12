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

	friend std::ostream& operator<<(std::ostream& os, ServerInfo const& x) {
		return os << x.name << ", " << x.hostname << ", " << x.port << std::endl;
	}
};

struct LibraryInfo {
	std::string name;
	std::string path;
	std::string cache_file;

	friend std::ostream& operator<<(std::ostream& os, LibraryInfo const& x) {
		return os << x.name << ", " << x.path << ", " << x.cache_file << std::endl;
	}
};

boost::filesystem::path get_config_file_name();
boost::filesystem::path get_database_file_name();
boost::filesystem::path get_library_folder_name();
boost::filesystem::path get_unique_cache_name();
boost::filesystem::path get_gst_folder_name();

int get_volume();
void set_volume(int volume);

std::vector<LibraryInfo> get_library_information();
void add_library(LibraryInfo info);
void clear_library();

std::vector<ServerInfo> get_servers();
void add_server(std::string name, std::string hostname, std::string port);
void clear_servers();

void print_config_dir();
void print_config_file_name();

}
