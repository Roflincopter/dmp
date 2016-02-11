#pragma once

#include "library-entry.hpp"
#include "load_info.hpp"

#include "fusion_outputter.hpp"

#include "archive.hpp"

#include "dmp_config.hpp"

#include <boost/filesystem/path.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/optional/optional.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/unordered_map.hpp>

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <cstdint>
#include <fstream>
#include <unordered_map>

namespace dmp_library {

struct EntryNotFound : public std::runtime_error {
	EntryNotFound();
};

struct EntryLocation {
	LibraryEntry entry;
	std::string location;

	template<typename Archive>
	void serialize(Archive& ar, const unsigned int)
	{
		ar & entry & location;
	}
};

struct Interrupted : std::runtime_error {
	
	Interrupted()
	: std::runtime_error("Loading of this library was interrupted")
	{}
};

struct Library
{
	std::shared_ptr<LoadInfo> load_info;
	
	typedef std::unordered_multimap<LibraryEntry, EntryLocation> library_t;
	//typedef std::multimap<std::hash<LibraryEntry>::result_type, EntryLocation> library_t;

	library_t library;

	Library();
	Library(library_t library);

	Library read_cache(std::string cache_path);
	void write_cache(std::string const& cache_path);

	boost::optional<LibraryEntry> build_library_entry(boost::filesystem::path p);
	Library build_library(boost::filesystem::path p);
	Library parse_directory(std::string const& directory_path);
	Library create(config::LibraryInfo info, bool use_cache = true, bool create_cache = true);

	template<typename Archive>
	void serialize(Archive& ar, const unsigned int)
	{
		ar & library;
	}

	std::string get_filename(const LibraryEntry& entry) const;
	void clear();
	void add_folder(config::LibraryInfo const& info);
	void load_library(std::vector<config::LibraryInfo> const& library_info);

	friend std::ostream& operator<<(std::ostream& os, Library const& l) {
		return os << l.library.size();
	}
};

}
