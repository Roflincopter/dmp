#pragma once

#include "library-entry.hpp"

#include <boost/filesystem/path.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>

#include <vector>
#include <map>

namespace dmp_library {

struct Library
{
	typedef std::vector<LibraryEntry> tracklist_t;
	typedef std::map<uint32_t, std::string> filemap_t;

	boost::filesystem::path path;
	tracklist_t tracklist;
	filemap_t filemap;

	Library();
	Library(boost::filesystem::path path, tracklist_t tracklist, filemap_t filemap);

	template<typename Archive>
	void serialize(Archive& ar, const unsigned int)
	{
		ar & tracklist & filemap;
	}

	friend std::ostream& operator<< (std::ostream& os, Library const& l)
	{
		for(auto&& e : l.tracklist) {
			os << l.filemap.at(e.id) << std::endl << e << std::endl;
		}
		return os;
	}
	
	std::string get_filename(LibraryEntry entry) const;
};

}
