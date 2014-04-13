#pragma once

#include "dmp_radio.hpp"
#include "client_endpoint.hpp"
#include "dmp_server_interface.hpp"

#include "connection.hpp"
#include "message_callbacks.hpp"
#include "number_pool.hpp"

#include <thread>

class DmpServer : public DmpServerInterface, public std::enable_shared_from_this<DmpServerInterface>
{
	boost::asio::io_service server_io_service;
	std::map<std::string, std::shared_ptr<ClientEndpoint>> connections;
	std::map<std::string, std::pair<std::thread, DmpRadio>> radios;
	boost::asio::deadline_timer debug_timer;
	std::shared_ptr<NumberPool> port_pool;

public:
	DmpServer();

	void run();
	void stop();
	void add_connection(dmp::Connection&& c);

	void handle_search(std::shared_ptr<ClientEndpoint> origin, message::SearchRequest sr);
	void handle_add_radio(std::shared_ptr<ClientEndpoint> origin, message::AddRadio ar);
	void handle_queue(message::Queue queue);
	void handle_radio_event(message::RadioEvent re);
	
	virtual void order_stream(std::string client, std::string radio_name, dmp_library::LibraryEntry entry, uint16_t port) override final;
	virtual void order_pause(std::string client, std::string radio_name) override final;
	virtual void order_play(std::string client, std::string radio_name) override final;
	virtual void order_stop(std::string client, std::string radio_name) override final;
};
