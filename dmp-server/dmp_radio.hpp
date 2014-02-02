#pragma once

#include "dmp-library.hpp"

#include "playlist.hpp"
#include "number_pool.hpp"

#include <gst/gst.h>

#include <stdexcept>
#include <iostream>
#include <memory>

class DmpRadio
{
private:
	std::string name;
	std::shared_ptr<NumberPool> port_pool;

	GMainLoop* loop;
	GstElement* pipeline;
	GstElement* source;
	GstElement* sink;
	GstBus *bus;

	uint16_t recv_port;
	uint16_t send_port;

	Playlist playlist;

public:

	DmpRadio();
	DmpRadio(std::string name, std::shared_ptr<NumberPool> port_pool);

	void run();

	uint16_t get_sender_port();
	uint16_t get_receiver_port();

	Playlist get_playlist();
	void queue(std::string queuer, std::string owner, dmp_library::LibraryEntry entry);
};

