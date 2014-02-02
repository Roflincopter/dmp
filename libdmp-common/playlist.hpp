#pragma once

#include <boost/fusion/adapted.hpp>

#include <string>

struct PlaylistEntry {
	std::string queuer;
	std::string owner;
	dmp_library::LibraryEntry entry;
	
	template <typename Archive>
	void serialize(Archive& ar, unsigned int)
	{
		ar & queuer & owner & entry;
	}
};

typedef std::vector<PlaylistEntry> Playlist;
