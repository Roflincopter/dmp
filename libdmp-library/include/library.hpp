#pragma once

#include "library-entry.hpp"

#include "fusion_outputter.hpp"

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

namespace config { struct LibraryInfo; }

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

struct Library
{
	typedef std::multimap<std::hash<LibraryEntry>::result_type, EntryLocation> library_t;

	library_t library;

	Library();
	Library(library_t library);

	static Library read_cache(std::string cache_path);
	static void write_cache(std::string const& cache_path, Library const& lib)
	{
		std::ofstream ofs(cache_path);
		if(ofs)
		{
			boost::archive::text_oarchive ar(ofs);
			ar & lib;
		}
		else
		{
			std::cout << "opening file " + cache_path + " for writing failed: make this a nice exception." << std::endl;
		}
		return;
	}

	static boost::optional<LibraryEntry> build_library_entry(boost::filesystem::path p);
	static Library build_library(boost::filesystem::path p);
	static Library parse_directory(std::string const& directory_path);
	static Library create(config::LibraryInfo info, bool use_cache = true, bool create_cache = true);

	template<typename Archive>
	void serialize(Archive& ar, const unsigned int)
	{
		ar & library;
	}

	std::string get_filename(LibraryEntry entry) const;
	void clear();
	void add_folder(const config::LibraryInfo &info);

	friend std::ostream& operator<<(std::ostream& os, Library const& l) {
		return os << l.library.size();
	}
};

}
