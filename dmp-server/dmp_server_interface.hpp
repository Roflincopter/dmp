#pragma once

#include "message.hpp"

#include <string>

struct DmpServerInterface {
	
	virtual void forward_receiver_action(std::string client, message::ReceiverAction) = 0;
	virtual void forward_sender_action(std::string client, message::SenderAction) = 0;
	virtual void order_stream(std::string client, std::string radio_name, dmp_library::LibraryEntry entry, uint16_t port) = 0;
	virtual void update_playlist(std::string radio, Playlist playlist) = 0;
};
