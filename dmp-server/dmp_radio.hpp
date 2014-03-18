#pragma once

#include "dmp-library.hpp"
#include "dmp_server_interface.hpp"
#include "playlist.hpp"
#include "number_pool.hpp"
#include "gstreamer_base.hpp"

#include <boost/optional.hpp>

#include <stdexcept>
#include <iostream>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>

class DmpRadio : public GStreamerBase
{
private:
	std::string name;
	std::weak_ptr<DmpServerInterface> server;
	std::shared_ptr<NumberPool> port_pool;
	std::unique_ptr<std::mutex> queue_mutex;
	std::unique_ptr<std::condition_variable> queue_filled;
	std::unique_ptr<std::mutex> radio_mutex;
	std::unique_ptr<std::condition_variable> radio_action;
	std::thread queue_thread;
	
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
	DmpRadio(std::string name, std::weak_ptr<DmpServerInterface> server, std::shared_ptr<NumberPool> port_pool);

	void run();

	uint16_t get_sender_port();
	uint16_t get_receiver_port();

	Playlist get_playlist();
	void play();
	void queue(std::string queuer, std::string owner, dmp_library::LibraryEntry entry);
};

