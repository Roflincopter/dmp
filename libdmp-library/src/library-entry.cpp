
#include "library-entry.hpp"

#include "fusion_lexicographical_compare.hpp"
#include "icu-ascii-transliterator.hpp"

#include <iomanip>
#include <sstream>

using namespace std;

namespace dmp_library {

LibraryEntry::LibraryEntry(string artist, string title, string album, std::uint32_t track, std::uint32_t length)
: artist(artist)
, ascii_artist()
, title(title)
, ascii_title()
, album(album)
, ascii_album()
, track(track)
, length(length)
, id(std::hash<LibraryEntry>()(*this))
{
	try {
		this->ascii_artist = transliterate_to_ascii(artist);
		this->ascii_title = transliterate_to_ascii(title);
		this->ascii_album = transliterate_to_ascii(album);
	} catch(...) {
		
	}
}

bool operator==(LibraryEntry const& lh, LibraryEntry const& rh)
{
	return
		lh.artist       == rh.artist       &&
		lh.ascii_artist == rh.ascii_artist &&
		lh.title        == rh.title        &&
		lh.ascii_title  == rh.ascii_title  &&
		lh.album        == rh.album        &&
		lh.ascii_album  == rh.ascii_album  &&
		lh.track        == rh.track        &&
		lh.length       == rh.length       &&
		lh.id           == rh.id;
}


ostream& operator<<(ostream& os, LibraryEntry const& le)
{
	return os
		<< "{" << std::endl
		<< "\t" << "UID: " << le.id << std::endl
		<< "\t" << "artist: " << le.artist << std::endl
		<< "\t" << "a_artist: " << le.ascii_artist << std::endl
		<< "\t" << "title: " << le.title << std::endl
		<< "\t" << "a_title: " << le.ascii_title << std::endl
		<< "\t" << "album: " << le.album << std::endl
		<< "\t" << "track: " << le.track << std::endl
		<< "\t" << "length" << le.length << std::endl
		<< "\t" << "a_album: " << le.ascii_album << std::endl
		<< "}" << std::endl;
}

bool operator==(LibraryEntry::Duration const& lh, LibraryEntry::Duration const& rh)
{
	return
		lh.hours   == rh.hours   &&
		lh.minutes == rh.minutes &&
		lh.seconds == rh.seconds;
}

std::ostream& operator<<(std::ostream& os, LibraryEntry::Duration const& dur)
{
	return os << dur.to_string();
}

bool operator<(LibraryEntry::Duration const& lh, LibraryEntry::Duration const& rh) {
	return lexicographical_compare(lh, rh);
}

LibraryEntry::Duration::Duration(std::uint32_t duration)
	: hours(duration / (60 * 60))
	, minutes((duration / 60) % 60)
	, seconds(duration % 60)
{

}

string LibraryEntry::Duration::to_string() const
{
	std::stringstream ss;
	if(hours) {
		ss << hours << "h" << ":" << std::setw(2) << std::setfill('0') << minutes << "m";
	}
	if(!hours && minutes) {
		ss << minutes << "m";
	}
	if(minutes) {
		ss << ":" << std::setw(2) << std::setfill('0') << seconds << "s";
	}
	if(!hours && !minutes && seconds) {
		ss << seconds << "s";
	}
	return ss.str();
}

bool operator<(const LibraryEntry &lh, const LibraryEntry &rh)
{
	return lexicographical_compare(lh, rh);
}

}
