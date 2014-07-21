#pragma once

#include "dmp_server_interface.hpp"
#include "dmp_radio_endpoint.hpp"

#include "number_pool.hpp"
#include "gstreamer_base.hpp"
#include "debug_macros.hpp"

#include "dmp-library.hpp"
#include "playlist.hpp"

#include <boost/optional.hpp>

#include <stdexcept>
#include <iostream>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>

class DmpRadio : public GStreamerBase
{
	struct TeeBranch {
		std::string pad_name;
		std::unique_ptr<GstPad, GStreamerRequestPadDeleter> pad;
		DmpRadioEndpoint endpoint;
		
		TeeBranch(std::string pad_name, std::unique_ptr<GstPad, GStreamerRequestPadDeleter>&& pad, DmpRadioEndpoint&& endpoint)
		: pad_name(pad_name)
		, pad(std::move(pad))
		, endpoint(std::move(endpoint))
		{
			DEBUG_COUT << this->endpoint.get_sink() << std::endl;
		}
		
		TeeBranch(TeeBranch&&) = default;
		
		~TeeBranch()
		{}
	};
	
	std::string name;
	std::weak_ptr<DmpServerInterface> server;
	std::shared_ptr<NumberPool> port_pool;

	std::unique_ptr<GstElement, GStreamerEmptyDeleter> source;
	std::unique_ptr<GstElement, GStreamerEmptyDeleter> buffer;
	std::unique_ptr<GstElement, GStreamerEmptyDeleter> parser;
	std::unique_ptr<GstElement, GStreamerEmptyDeleter> tee;
//	std::unique_ptr<GstElement, GStreamerEmptyDeleter> sink;
	
	std::unique_ptr<GstPadTemplate, GStreamerObjectDeleter> tee_src_pad_template;
	
	std::map<std::string, TeeBranch> branches;
	
	std::unique_ptr<std::mutex> radio_mutex;

	uint16_t recv_port;

	Playlist playlist;
	
	bool stopped = true;
	
public:
	DmpRadio(std::string name, std::weak_ptr<DmpServerInterface> server, std::shared_ptr<NumberPool> port_pool);

	DmpRadio(DmpRadio&& r) = default;
	DmpRadio& operator=(DmpRadio&& r) = default;

	void listen();
	void add_listener(std::string name);
	
	uint16_t get_sender_port();
	uint16_t get_receiver_port(std::string name);

	Playlist get_playlist();
	
	void stop();
	void play();
	void pause();
	void next();
	
	virtual void eos_reached() override final;
	
	void queue(std::string queuer, std::string owner, dmp_library::LibraryEntry entry);
};

