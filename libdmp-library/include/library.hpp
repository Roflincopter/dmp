#pragma once

#include "library-entry.hpp"

#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>

#include <vector>
#include <map>

namespace dmp_library {

struct Library
{
	typedef std::vector<LibraryEntry> tracklist_t;
	typedef std::map<uint32_t, std::string> filemap_t;

	tracklist_t tracklist;
	filemap_t filemap;

	Library();
	Library(tracklist_t tracklist, filemap_t filemap);

	template<typename Archive>
	void serialize(Archive& ar, const unsigned int)
	{
		ar & tracklist & filemap;
	}
};

}
