#pragma once

#include "message.hpp"

#include "dmp-library.hpp"

class PlaylistsModel
{
protected:
	std::map<std::string, message::Playlist> playlists;
	std::string current_radio;

public:
	PlaylistsModel();

	int row_count() const;
	int column_count() const;

	std::string header_data(int section) const;
	std::string get_cell(int row, int column) const;

	virtual void update_playlist(std::string radio_name, std::vector<std::tuple<std::string, std::string, dmp_library::LibraryEntry>> playlist);
	virtual void set_current_radio(std::string radio_name);
};
