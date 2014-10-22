#pragma once

#include "connection.hpp"
#include "message_callbacks.hpp"
#include "message_switch.hpp"

#include "debug_macros.hpp"

#include <boost/asio/deadline_timer.hpp>

class ClientEndpoint : public std::enable_shared_from_this<ClientEndpoint>
{
	std::string name;

	//ping_timer depends on the io_service of the connection so the pings stop when the connection dies.
	//Leave them in this order or make them independant.
	Connection connection;
	std::unique_ptr<boost::asio::deadline_timer> ping_timer;

	message::Ping last_ping;
	message::DmpCallbacks callbacks;
	MessageSwitch message_switch;
	std::function<void()> terminate_connection;

public:

	ClientEndpoint(Connection&& conn);

	void run();
	void keep_alive();
	void listen_requests();
	void cancel_pending_asio();

	void set_terminate_connection(std::function<void()> f);
	std::string get_name() {return name;}
	void set_name(std::string name) {this->name = name;}

	message::DmpCallbacks& get_callbacks();

	template <typename T>
	void forward(T x)
	{
		connection.send_encrypted(x);
	}

	void search(std::function<void(message::SearchResponse)> cb, message::SearchRequest sr);

	void handle_request(message::Type t);
	void handle_pong(message::Pong p);
	void handle_bye(message::Bye);
};
