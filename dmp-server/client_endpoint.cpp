#include "client_endpoint.hpp"

#include "message.hpp"
#include "fusion_outputter.hpp"

#include "debug_macros.hpp"

ClientEndpoint::ClientEndpoint(Connection&& conn, std::weak_ptr<boost::asio::io_service> ios)
: name()
, connection(std::move(conn))
, ping_timer(new boost::asio::deadline_timer(*ios.lock()))
, last_ping()
, callbacks(std::bind(&ClientEndpoint::listen_requests, this), message::DmpCallbacks::Callbacks_t{})
, message_switch(make_message_switch(callbacks, connection))
, terminate_connection()
{
	callbacks.
		set(message::Type::Pong, std::function<bool(message::Pong)>(std::bind(&ClientEndpoint::handle_pong, this, std::placeholders::_1)));
	
	listen_requests();
	keep_alive();
}

message::DmpCallbacks& ClientEndpoint::get_callbacks()
{
	return callbacks;
}

void ClientEndpoint::search(std::function<void(message::SearchResponse)> cb, message::SearchRequest sr)
{
	callbacks.set(message::Type::SearchResponse, [cb](message::SearchResponse sr){cb(sr); return true;});
	forward(sr);
}

void ClientEndpoint::handle_request(message::Type t)
{
	message_switch.handle_message(t);
}

bool ClientEndpoint::handle_pong(message::Pong p)
{
	if(p.payload != last_ping.payload) {
		terminate_connection();
	}
	return true;
}

void ClientEndpoint::listen_requests()
{
	std::function<void(message::Type)> cb = std::bind(&ClientEndpoint::handle_request, this, std::placeholders::_1);
	connection.async_receive_type(cb);
}

void ClientEndpoint::set_terminate_connection(std::function<void ()> f)
{
	terminate_connection = f;
}

void ClientEndpoint::set_our_keys(std::vector<uint8_t> priv, std::vector<uint8_t> pub)
{
	connection.set_our_keys(priv, pub);
}

void ClientEndpoint::set_their_key(std::vector<uint8_t> opub) {
	connection.set_their_key(opub);
}

bool ClientEndpoint::handle_bye(message::Bye)
{
	connection.stop_encryption();
	forward(message::ByeAck());
	ping_timer->cancel();
	if(terminate_connection) {
		terminate_connection();
	}
	return false;
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
		forward(last_ping);
		keep_alive();
	};
	ping_timer->async_wait(cb);
}
