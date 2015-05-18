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

public:

	ClientEndpoint(Connection&& conn, std::weak_ptr<boost::asio::io_service> ios);

	std::function<void()> terminate_connection;

	void run();
	void keep_alive();
	void listen_requests();

	void set_terminate_connection(std::function<void()> f);
	std::string get_name() {return name;}
	void set_name(std::string name) {this->name = name;}

	message::DmpCallbacks& get_callbacks();

	template <typename T>
	void forward(T x)
	{
		try {
			connection.send(x);
		} catch(boost::system::system_error&) {
			terminate_connection();
			throw;
		}
	}

	void search(std::function<void(message::SearchResponse)> cb, message::SearchRequest sr);

	void handle_request(message::Type t);
	bool handle_pong(message::Pong p);
	bool handle_bye(message::Bye);
};
