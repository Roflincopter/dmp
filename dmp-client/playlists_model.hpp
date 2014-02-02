#pragma once

#include "message.hpp"
#include "playlist.hpp"

#include "dmp-library.hpp"

class PlaylistsModel
{
protected:
	std::map<std::string, Playlist> playlists;
	std::string current_radio;

public:
	PlaylistsModel();

	int row_count() const;
	int column_count() const;

	std::string header_data(int section) const;
	std::string get_cell(int row, int column) const;

	virtual void update_playlist(std::string radio_name, Playlist playlist);
	virtual void append(std::string radio_name, Playlist playlist);

	virtual void set_current_radio(std::string radio_name);
	virtual void create_radio(std::string radio_name);
};
