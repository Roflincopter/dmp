#include "client_endpoint.hpp"

#include "message.hpp"
#include "fusion_outputter.hpp"

#include "debug_macros.hpp"

ClientEndpoint::ClientEndpoint(std::string name, Connection&& conn, std::function<void()> terminate_connection)
: name(name)
, connection(std::move(conn))
, ping_timer(new boost::asio::deadline_timer(*connection.io_service))
, last_ping()
, callbacks(std::bind(&ClientEndpoint::listen_requests, this), message::DmpCallbacks::Callbacks_t{})
, message_switch(make_message_switch(callbacks, connection))
, terminate_connection(terminate_connection)
{
	callbacks.
		set(message::Type::Pong, std::function<void(message::Pong)>(std::bind(&ClientEndpoint::handle_pong, this, std::placeholders::_1))).
		set(message::Type::Bye, std::function<void(message::Bye)>(std::bind(&ClientEndpoint::handle_bye, this, std::placeholders::_1)))
;
	
	listen_requests();
	keep_alive();
}

message::DmpCallbacks& ClientEndpoint::get_callbacks()
{
	return callbacks;
}

void ClientEndpoint::search(std::function<void(message::SearchResponse)> cb, message::SearchRequest sr)
{
	callbacks.set(message::Type::SearchResponse, cb);
	connection.send(sr);
}

void ClientEndpoint::handle_request(message::Type t)
{
	message_switch.handle_message(t);
}

void ClientEndpoint::handle_pong(message::Pong p)
{
	if(p.payload != last_ping.payload) {
		terminate_connection();
	}
}

void ClientEndpoint::listen_requests()
{
	std::function<void(message::Type)> cb = std::bind(&ClientEndpoint::handle_request, this, std::placeholders::_1);
	connection.async_receive_type(cb);
}

void ClientEndpoint::cancel_pending_asio()
{
	callbacks.stop_refresh();
	ping_timer->cancel();
}

void ClientEndpoint::handle_bye(message::Bye)
{
	terminate_connection();
}

void ClientEndpoint::keep_alive()
{
	ping_timer->expires_from_now(boost::posix_time::seconds(10));
	auto cb = [this](boost::system::error_code const& ec)
	{
		if(ec) {
			if(ec.value() == boost::system::errc::operation_canceled) {
				return;
			}
			throw std::runtime_error("something went wrong in the keep alive timer.");
		}

		last_ping = message::Ping();
		connection.send(last_ping);
		keep_alive();
	};
	ping_timer->async_wait(cb);
}
