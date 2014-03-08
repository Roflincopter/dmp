#pragma once

#include "message.hpp"
#include "playlist.hpp"

#include "dmp-library.hpp"

class PlaylistsModel
{
	static constexpr size_t number_of_library_entry_members = boost::fusion::result_of::size<dmp_library::LibraryEntry>::type::value;
	static constexpr size_t number_of_playlist_entry_members = boost::fusion::result_of::size<PlaylistEntry>::type::value;
	
protected:
	std::map<std::string, Playlist> playlists;
	std::string current_radio;

public:
	PlaylistsModel();

	int row_count() const;
	int column_count() const;

	std::string header_data(int section) const;
	std::string get_cell(int row, int column) const;

	virtual void update(std::string radio_name, Playlist playlist);
	virtual void append(std::string radio_name, Playlist playlist);
	virtual void reset(std::string radio_name);

	virtual void set_current_radio(std::string radio_name);
	virtual void create_radio(std::string radio_name);
};
