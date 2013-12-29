#include "dmp_client.hpp"

#include "message_outputter.hpp"


DmpClient::DmpClient(std::string name, std::string host, dmp::Connection&& conn)
: name(name)
, host(host)
, connection(std::move(conn))
, last_sent_ping()
, lib()
, delegates()
, sender()
, receiver()
, callbacks(std::bind(&DmpClient::listen_requests, this))
, message_switch(make_message_switch(callbacks, connection))
{
	callbacks.
		set(message::Type::Ping, std::function<void(message::Ping)>(std::bind(&DmpClient::handle_ping, this, std::placeholders::_1))).
		set(message::Type::Pong, std::function<void(message::Ping)>(std::bind(&DmpClient::handle_pong, this, std::placeholders::_1))).
		set(message::Type::NameRequest, std::function<void(message::NameRequest)>(std::bind(&DmpClient::handle_name_request, this, std::placeholders::_1))).
		set(message::Type::SearchRequest, std::function<void(message::SearchRequest)>(std::bind(&DmpClient::handle_search_request, this, std::placeholders::_1))).
		set(message::Type::ByeAck, std::function<void(message::ByeAck)>(std::bind(&DmpClient::handle_bye_ack, this, std::placeholders::_1))).
		set(message::Type::AddRadioResponse, std::function<void(message::AddRadioResponse)>(std::bind(&DmpClient::handle_add_radio_response, this, std::placeholders::_1))).
		set(message::Type::ListenConnectionRequest, std::function<void(message::ListenConnectionRequest)>(std::bind(&DmpClient::handle_listener_connection_request, this, std::placeholders::_1))).
		set(message::Type::Radios, std::function<void(message::Radios)>(std::bind(&DmpClient::handle_radios, this, std::placeholders::_1))).
		set(message::Type::AddRadio, std::function<void(message::AddRadio)>(std::bind(&DmpClient::handle_add_radio, this, std::placeholders::_1)));
}

void DmpClient::add_delegate(std::weak_ptr<DmpClientUiDelegate> delegate)
{
	delegates.push_back(delegate);
}

void DmpClient::run()
{
	listen_requests();
	connection.io_service->run();
}

void DmpClient::stop()
{
	connection.io_service->stop();
}

void DmpClient::index(std::string path)
{
	lib = dmp_library::create_library(path);
}

void DmpClient::add_radio(std::string radio_name)
{
	connection.send(message::AddRadio(radio_name));
}

void DmpClient::handle_request(message::Type t)
{
	message_switch.handle_message(t);
}

void DmpClient::listen_requests()
{
	std::function<void(message::Type)> cb = std::bind(&DmpClient::handle_request, this, std::placeholders::_1);
	connection.async_receive_type(cb);
}

void DmpClient::search(std::string query)
{
	callbacks.set(message::Type::SearchResponse, std::function<void(message::SearchResponse)>(std::bind(&DmpClient::handle_search_response, this, query, std::placeholders::_1)));

	try {
		dmp_library::parse_query(query);
	} catch (dmp_library::ParseError err) {
		call_on_delegates(delegates, &DmpClientUiDelegate::query_parse_error, err);
		return;
	}

	call_on_delegates(delegates, &DmpClientUiDelegate::new_search);
	message::SearchRequest q(query);
	connection.send(q);
}

void DmpClient::send_bye()
{
	message::Bye b;
	connection.send(b);
}

void DmpClient::handle_ping(message::Ping ping)
{
	message::Pong pong(ping);
	connection.send(pong);
}

void DmpClient::handle_pong(message::Pong pong)
{
	assert(last_sent_ping.payload != "");
	if (last_sent_ping.payload != pong.payload) {
		stop();
	}
}

void DmpClient::handle_name_request(message::NameRequest name_req)
{
	message::NameResponse name_res(name);
	connection.send(name_res);
}

void DmpClient::handle_search_request(message::SearchRequest search_req)
{
	dmp_library::LibrarySearcher searcher(lib);
	message::SearchResponse response(search_req.query, searcher.search(search_req.query), name);
	connection.send(response);
}

void DmpClient::handle_search_response(std::string query, message::SearchResponse search_res)
{
	//If the result is out of date, ignore it.
	if(query != search_res.query) {
		return;
	}

	call_on_delegates(delegates, &DmpClientUiDelegate::search_results, search_res);
}

void DmpClient::handle_bye_ack(message::ByeAck)
{
	call_on_delegates(delegates, &DmpClientUiDelegate::bye_ack_received);
}

void DmpClient::handle_add_radio_response(message::AddRadioResponse response)
{
	if(response.succes) {
		call_on_delegates(delegates, &DmpClientUiDelegate::add_radio_succes, response);
	} else {
		call_on_delegates(delegates, &DmpClientUiDelegate::add_radio_failed, response);
	}
}

void DmpClient::handle_listener_connection_request(message::ListenConnectionRequest req)
{
	receiver.stop();
	if(receiver_thread.joinable()) {
		receiver_thread.join();
	}
	receiver_thread = std::thread(std::bind(&DmpReceiver::connect, receiver, host, req.port));
}

void DmpClient::handle_radios(message::Radios radios)
{
	call_on_delegates(delegates, &DmpClientUiDelegate::radios_update, radios);
}

DmpClient::~DmpClient() {
	receiver.stop();
	if(receiver_thread.joinable()) {
		receiver_thread.join();
	}
}

void DmpClient::handle_add_radio(message::AddRadio added_radio)
{
	call_on_delegates(delegates, &DmpClientUiDelegate::radio_added, added_radio);
}
