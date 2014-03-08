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
	
	friend std::ostream& operator<<(std::ostream& os, PlaylistEntry const& pe);
};

inline std::ostream& operator<<(std::ostream& os, PlaylistEntry const& pe) {
	os << "queuer: " << pe.queuer << ", owner: " << pe.owner << ", entry:\n" << pe.entry << std::endl;
	return os;
}

BOOST_FUSION_ADAPT_STRUCT (
	PlaylistEntry,
	(std::string, queuer)
	(std::string, owner)
//	(dmp_library::LibraryEntry, entry)
)

typedef std::vector<PlaylistEntry> Playlist;
