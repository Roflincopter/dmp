#pragma once

#include "playlist.hpp"
#include "dmp-library.hpp"

#include <string>

struct DmpServerInterface {
	
	virtual void order_stream(std::string client, std::string radio_name, dmp_library::LibraryEntry entry, uint16_t port) = 0;
	virtual void order_pause(std::string client, std::string radio_name) = 0;
	virtual void order_play(std::string client, std::string radio_name) = 0;
	virtual void order_stop(std::string client, std::string radio_name) = 0;
	virtual void order_reset(std::string client, std::string radio_name) = 0;
	virtual void update_playlist(std::string radio, Playlist playlist) = 0;
};
