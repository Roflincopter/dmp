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

	std::unique_ptr<GstElement, GStreamerEmptyDeleter> source;
	std::unique_ptr<GstElement, GStreamerEmptyDeleter> buffer;
	std::unique_ptr<GstElement, GStreamerEmptyDeleter> parser;
	std::unique_ptr<GstElement, GStreamerEmptyDeleter> sink;
	
	std::unique_ptr<std::mutex> radio_mutex;

	uint16_t recv_port;
	uint16_t send_port;

	Playlist playlist;

public:
	DmpRadio(std::string name, std::weak_ptr<DmpServerInterface> server, std::shared_ptr<NumberPool> port_pool);

	DmpRadio(DmpRadio&& r) = default;
	DmpRadio& operator=(DmpRadio&& r) = default;

	void listen();
	
	uint16_t get_sender_port();
	uint16_t get_receiver_port();

	Playlist get_playlist();
	
	void stop();
	void play();
	void pause();
	void next();
	
	virtual void eos_reached() override final;
	
	void queue(std::string queuer, std::string owner, dmp_library::LibraryEntry entry);
};

