#include "dmp_config.hpp"

#include "debug_macros.hpp"

#include <boost/filesystem.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <fstream>

#if defined( _WIN32 ) || defined( _WIN64 )
boost::filesystem::path get_config_dir() {
	if(!std::getenv("APPDATA")) {
		throw std::runtime_error("Environment variable \"APPDATA\" is not set, could not find suitable config home.");
	}
	boost::filesystem::path appdata(std::getenv("APPDATA"));
	return appdata / boost::path("dmp");
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

boost::filesystem::path get_config_file_name() {
	return get_config_dir() / "config";
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
	{
		DEBUG_COUT << "reading config" << std::endl;
	}

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

boost::optional<boost::property_tree::ptree&> get_library_information() {
	return config.get_child_optional(library_key);
}

void config_add_library(std::string name, std::string path, std::string cache_file) {
	DEBUG_COUT << "adding library" << std::endl;
	auto library = config.get_child_optional(library_key);

	boost::property_tree::ptree inner;
	inner.put("name", name);
	inner.put("path", path);
	inner.put("cache_file", cache_file);

	if(!library) {
		boost::property_tree::ptree array;
		config.add_child(library_key, array);
		library = config.get_child_optional(library_key);
	}

	library.get().push_back(std::make_pair("", inner));
}

void print_config_dir() {
	DEBUG_COUT << get_config_dir() << std::endl;
}

void print_config_file_name() {
	DEBUG_COUT << get_config_file_name() << std::endl;
}
