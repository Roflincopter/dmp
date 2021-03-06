#pragma once

#include <library-entry.hpp>

#include <boost/fusion/adapted/struct/adapt_struct.hpp>

#include <stdint.h>
#include <string>
#include <ostream>
#include <vector>

typedef uint32_t PlaylistId;

struct PlaylistEntry {
	struct QueuerOwnerPair{
		std::string& queuer;
		std::string& owner;
	};

	std::string queuer;
	std::string owner;
	PlaylistId playlist_id;
	uint32_t folder_id;
	dmp_library::LibraryEntry entry;
	
	PlaylistEntry() = default;
	
	PlaylistEntry(std::string queuer, std::string owner, dmp_library::LibraryEntry entry)
	: queuer(queuer)
	, owner(owner)
	, playlist_id(-1)
	, entry(entry)
	{}
	
	template <typename Archive>
	void serialize(Archive& ar, unsigned int)
	{
		ar & queuer & owner & playlist_id & entry;
	}
	
	friend std::ostream& operator<<(std::ostream& os, PlaylistEntry const& pe);
};

inline std::ostream& operator<<(std::ostream& os, PlaylistEntry const& pe) {
	os << "queuer: " << pe.queuer << ", owner: " << pe.owner << ", folder_id: " << pe.folder_id << ", entry:\n" << pe.entry << std::endl;
	return os;
}

BOOST_FUSION_ADAPT_STRUCT (
	PlaylistEntry,
	(std::string, queuer)
	(std::string, owner)
	(uint32_t, playlist_id)
	(dmp_library::LibraryEntry, entry)
)

BOOST_FUSION_ADAPT_STRUCT (
	PlaylistEntry::QueuerOwnerPair,
	(std::string&, queuer)
	(std::string&, owner)
)

typedef std::vector<PlaylistEntry> Playlist;
