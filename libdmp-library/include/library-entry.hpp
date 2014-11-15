#pragma once

#include "icu-ascii-transliterator.hpp"
#include "fusion_lexicographical_compare.hpp"

#include <iostream>
#include <string>
#include <cstdint>

#include <boost/fusion/adapted.hpp>

namespace dmp_library
{

struct LibraryEntry
{
	struct Duration
	{
		uint32_t hours;
		uint32_t minutes;
		uint32_t seconds;
		
		Duration() = default;
		
		friend bool operator<(Duration const& lh, Duration const& rh) {
			return lexicographical_compare(lh, rh);
		}

		Duration(std::uint32_t duration)
		: hours(duration / (60 * 60))
		, minutes((duration / 60) % 60)
		, seconds(duration % 60)
		{
			
		}
		
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
	uint32_t id;

	template<typename Archive>
	void serialize(Archive& ar, const unsigned int)
	{
		ar & artist & ascii_artist & title & ascii_title & album & ascii_album & track & length & id;
	}

private:
	static uint32_t next_id;
};

std::ostream& operator<<(std::ostream& os, LibraryEntry const& le);
std::ostream& operator<<(std::ostream& os, LibraryEntry::Duration const& dur);

bool operator==(LibraryEntry::Duration const& lh, LibraryEntry::Duration const& rh);
bool operator==(LibraryEntry const& lh, LibraryEntry const& rh);

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
	(uint32_t, id)
)
