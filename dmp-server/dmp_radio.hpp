#pragma once

#include "dmp-library.hpp"

#include "number_pool.hpp"

#include <gst/gst.h>

#include <stdexcept>
#include <iostream>
#include <memory>

class DmpRadio
{
	std::string name;
	std::shared_ptr<NumberPool> port_pool;

	GMainLoop* loop;
	GstElement* pipeline;
	GstElement* source;
	GstElement* sink;
	GstBus *bus;

	uint16_t recv_port;
	uint16_t send_port;

public:

	DmpRadio();
	DmpRadio(std::string name, std::shared_ptr<NumberPool> port_pool);

	void run();

	uint16_t get_sender_port();
	uint16_t get_receiver_port();

	std::vector<dmp_library::LibraryEntry> get_playlist();
};

