#pragma once

#include "message.hpp"
#include "playlist.hpp"
#include "friendly_fusion.hpp"

#include "dmp-library.hpp"

#include <boost/any.hpp>

class PlaylistsModel
{
	static constexpr size_t number_of_library_entry_members = friendly_fusion::result_of::size<dmp_library::LibraryEntry>::type::value;
	static constexpr size_t number_of_playlist_entry_members = friendly_fusion::result_of::size<PlaylistEntry>::type::value;
	
protected:
	std::map<std::string, Playlist> playlists;
	std::string current_radio;

public:
	PlaylistsModel();

	typedef boost::fusion::joint_view<dmp_library::LibraryEntry, PlaylistEntry> ElementType;
	
	int row_count() const;
	int column_count() const;

	std::string header_data(int section) const;
	boost::any get_cell(int row, int column) const;

	virtual void update(std::string radio_name, Playlist playlist);
	virtual void append(std::string radio_name, Playlist playlist);
	virtual void reset(std::string radio_name);

	virtual void set_current_radio(std::string radio_name);
	virtual void create_radio(std::string radio_name);
};
