#pragma once

#include "playlist.hpp"

#include <string>
#include <stdint.h>

namespace dmp_library { struct LibraryEntry; }

namespace message { struct ReceiverAction; }

namespace message { struct SenderAction; }

struct DmpServerInterface {
	
	virtual void forward_receiver_action(std::string client, message::ReceiverAction) = 0;
	virtual void forward_sender_action(std::string client, message::SenderAction) = 0;
	virtual void order_stream(std::string client, std::string radio_name, uint32_t folder_id, dmp_library::LibraryEntry entry, uint16_t port) = 0;
	virtual void update_playlist(std::string radio, Playlist playlist) = 0;

	virtual ~DmpServerInterface(){}
};
