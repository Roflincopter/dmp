#pragma once

#include "library-entry.hpp"

#include "dmp_config.hpp"

#include "fusion_outputter.hpp"

#include <taglib/fileref.h>
#include <taglib/tag.h>

#include <boost/filesystem.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>

#include <vector>
#include <map>
#include <fstream>
#include <stdexcept>

namespace dmp_library {

struct LibraryFolder
{
	typedef std::vector<LibraryEntry> tracklist_t;
	typedef std::map<uint32_t, std::string> filemap_t;

	boost::filesystem::path path;
	tracklist_t tracklist;
	filemap_t filemap;

	LibraryFolder();
	LibraryFolder(boost::filesystem::path path, tracklist_t tracklist, filemap_t filemap);

	static LibraryFolder read_cache(std::string cache_path);
	static void write_cache(std::string const& cache_path, LibraryFolder const& lib)
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
	static LibraryFolder build_library(boost::filesystem::path p);
	static LibraryFolder parse_directory(std::string const& directory_path);
	static LibraryFolder create(config::LibraryInfo info, bool use_cache = true, bool create_cache = true);

	template<typename Archive>
	void serialize(Archive& ar, const unsigned int)
	{
		ar & tracklist & filemap;
	}

	friend std::ostream& operator<< (std::ostream& os, LibraryFolder const& l)
	{
		for(auto&& e : l.tracklist) {
			os << l.filemap.at(e.id) << std::endl << e << std::endl;
		}
		return os;
	}
};

struct Library {
	typedef std::map<uint32_t, LibraryFolder> library_t;
	library_t library;

	void clear();
	void add_folder(config::LibraryInfo info);

	std::string get_filename(uint32_t folder_id, LibraryEntry entry) const;
	
	friend std::ostream& operator<< (std::ostream& os, Library const& l) {
		return os << l.library;
	}
};

}
