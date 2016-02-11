#pragma once

#include <boost/fusion/adapted/struct/adapt_struct.hpp>

#include <iostream>
#include <string>
#include <cstdint>
#include <functional>
#include <sstream>

namespace dmp_library
{

struct LibraryEntry;

}

namespace std {

template<>
struct hash<dmp_library::LibraryEntry>;

}

namespace dmp_library {

struct LibraryEntry
{
	struct Duration
	{
		uint32_t hours;
		uint32_t minutes;
		uint32_t seconds;
		
		Duration() = default;
		Duration(std::uint32_t duration);
		
		std::string to_string() const;
		
		template <typename Archive>
		void serialize(Archive& ar, const unsigned int)
		{
			ar & hours & minutes & seconds;
		}
	};
	
	LibraryEntry() = default;

	LibraryEntry(std::string artist, std::string title, std::string album, std::uint32_t track, std::uint32_t length);

	LibraryEntry(LibraryEntry const&) = default;
	LibraryEntry& operator=(LibraryEntry const&) = default;
	LibraryEntry& operator=(LibraryEntry&&) = default;

	std::string artist;
	std::string ascii_artist;
	std::string title;
	std::string ascii_title;
	std::string album;
	std::string ascii_album;
	uint32_t track;
	Duration length;
	//std::hash<std::string>::result_type id;

	template<typename Archive>
	void serialize(Archive& ar, const unsigned int)
	{
		ar & artist & ascii_artist & title & ascii_title & album & ascii_album & track & length;// & id;
	}
};

std::ostream& operator<<(std::ostream& os, LibraryEntry const& le);
std::ostream& operator<<(std::ostream& os, LibraryEntry::Duration const& dur);

bool operator<(LibraryEntry const& lh, LibraryEntry const& rh);
bool operator==(LibraryEntry const& lh, LibraryEntry const& rh);

bool operator<(LibraryEntry::Duration const& lh, LibraryEntry::Duration const& rh);
bool operator==(LibraryEntry::Duration const& lh, LibraryEntry::Duration const& rh);

}

namespace std {

template<>
struct hash<dmp_library::LibraryEntry> {

	typedef std::hash<std::string>::result_type result_type;

	std::hash<std::string>::result_type operator()(dmp_library::LibraryEntry const& entry) const {
		std::stringstream ss;
		ss << entry.artist << entry.title << entry.album << entry.track << entry.length;
		return hash<string>()(ss.str());
	}
};

}

BOOST_FUSION_ADAPT_STRUCT
(
	dmp_library::LibraryEntry::Duration,
	(uint32_t, hours)
	(uint32_t, minutes)
	(uint32_t, seconds)
)

BOOST_FUSION_ADAPT_STRUCT
(
	dmp_library::LibraryEntry,
	(std::string, artist)
	(std::string, ascii_artist)
	(std::string, title)
	(std::string, ascii_title)
	(std::string, album)
	(std::string, ascii_album)
	(uint32_t, track)
	(dmp_library::LibraryEntry::Duration, length)
//	(std::hash<std::string>::result_type, id)
)
