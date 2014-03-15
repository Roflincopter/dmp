
#include "library-entry.hpp"

#include <sstream>
#include <iomanip>

using namespace std;

namespace dmp_library {

uint32_t LibraryEntry::next_id = 0;

LibraryEntry::LibraryEntry(string artist, string title, string album, std::uint32_t track, std::uint32_t length)
: artist(artist)
, ascii_artist(transliterate_to_ascii(artist))
, title(title)
, ascii_title(transliterate_to_ascii(title))
, album(album)
, ascii_album(transliterate_to_ascii(album))
, track(track)
, length(length)
, id(next_id++)
{}

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

string LibraryEntry::Duration::to_string() const
{
	std::stringstream ss;
	ss << hours << ":" << std::setw(2) << std::setfill('0') << minutes << ":" << std::setw(2) << std::setfill('0') << seconds;
	return ss.str();
}

}
