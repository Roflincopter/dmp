#include "dmp_config.hpp"

#include "debug_macros.hpp"

#include <boost/filesystem.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <fstream>

namespace config {

#if defined( _WIN32 ) || defined( _WIN64 )
boost::filesystem::path get_config_dir() {
	if(!std::getenv("APPDATA")) {
		throw std::runtime_error("Environment variable \"APPDATA\" is not set, could not find suitable config home.");
	}
	boost::filesystem::path appdata(std::getenv("APPDATA"));
	return appdata / boost::filesystem::path("dmp");
}
#endif

#if defined( __unix__ ) && !defined( __APPLE__) && !defined( __MACH__ )
boost::filesystem::path get_config_dir() {
	boost::filesystem::path config_home;
	boost::filesystem::path path;
	if(!std::getenv("XDG_CONFIG_HOME")) {
		if(!std::getenv("HOME")) {
			throw std::runtime_error("Neither \"XDG_CONFIG_HOME\" nor \"HOME\" environment variable are set, could not find suitable config home.");
		} else {
			config_home = getenv("HOME");
			path = config_home / boost::filesystem::path(".config/dmp");
		}
	} else {
		config_home = getenv("XDG_CONFIG_HOME");
		path = config_home / boost::filesystem::path("dmp");
	}
	return path;
}
#endif

#if defined( __APPLE__ ) && defined( __MACH__ )
boost::filesystem::path get_config_dir() {
	boost::filesystem::path path;
	if(!std::getenv("HOME")) {
		throw std::runtime_error("\"HOME\" environment variable is not set, could not find a suitable config home.");
	} else {
		path = boost::filesystem::path(std::getenv("HOME"));
		path /= boost::filesystem::path("Library/Application Support/dmp");
	}
	return path;
}
#endif

boost::filesystem::path get_or_create_config_dir() {
	boost::filesystem::path config_dir = get_config_dir();
	if(!boost::filesystem::exists(config_dir)) {
		boost::filesystem::create_directory(config_dir);
	}
	return config_dir;
}

boost::filesystem::path get_or_create_library_dir() {
	boost::filesystem::path library_dir = get_config_dir() / "library";
	if(!boost::filesystem::exists(library_dir)) {
		boost::filesystem::create_directory(library_dir);
	}
	return library_dir;
}

boost::filesystem::path get_config_file_name() {
	return get_or_create_config_dir() / "config";
}

boost::filesystem::path get_database_file_name() {
	return get_or_create_config_dir() / "server.db";
}

boost::filesystem::path get_library_folder_name() {
	return get_or_create_library_dir();
}

boost::property_tree::ptree create_or_open_config() {
	boost::filesystem::path config_file = get_config_file_name();
	if(!boost::filesystem::exists(config_file)) {
		return boost::property_tree::ptree();
	} else {
		boost::property_tree::ptree config;
		std::ifstream config_stream(config_file.string());
		boost::property_tree::read_json(config_stream, config);
		return config;
	}
}

struct DmpConfig : public boost::property_tree::ptree {

	DmpConfig()
	:boost::property_tree::ptree(create_or_open_config())
	{}

	~DmpConfig(){
		if(!boost::filesystem::exists(get_config_file_name())) {
			boost::filesystem::create_directories(get_config_file_name().parent_path());
		}

		std::ofstream config_stream(get_config_file_name().string());
		boost::property_tree::write_json(config_stream, *this);
	}
};

static DmpConfig config;
static const std::string library_key = "library";
static const std::string servers_key = "servers";
static const std::string volume_key = "volume";

boost::property_tree::ptree& append_array_element_to_key(std::string key)
{
	auto top_element = config.get_child_optional(key);
	
	boost::property_tree::ptree inner;
	
	if(!top_element) {
		boost::property_tree::ptree array;
		config.add_child(key, array);
		top_element = config.get_child(key);
	}
	
	top_element.get().push_back(std::make_pair("", inner));
	return top_element.get().back().second;
}

boost::property_tree::ptree& get_or_create_child(std::string key) {
	auto element = config.get_child_optional(key);
	if(!element) {
		config.add_child(key, boost::property_tree::ptree());
	}
	return config.get_child(key);
}

void set_volume(int volume)
{
	config.put(volume_key, volume);
}

boost::property_tree::ptree get_volume()
{
	return get_or_create_child(volume_key);
}

boost::property_tree::ptree get_library_information() {
	return get_or_create_child(library_key);
}

void add_library(std::string name, std::string path, std::string cache_file) {

	auto& new_elem = append_array_element_to_key(library_key);
	new_elem.put("name", name);
	new_elem.put("path", path);
	new_elem.put("cache_file", cache_file);
}

boost::property_tree::ptree get_servers()
{
	return get_or_create_child(servers_key);
}

void add_server(std::string name, std::string hostname, std::string port)
{
	auto& new_elem = append_array_element_to_key(servers_key);
	new_elem.put("name", name);
	new_elem.put("hostname", hostname);
	new_elem.put("port", port);
}

void clear_servers() {
	config.erase(servers_key);
}

void print_config_dir() {
	DEBUG_COUT << get_config_dir() << std::endl;
}

void print_config_file_name() {
	DEBUG_COUT << get_config_file_name() << std::endl;
}

}
